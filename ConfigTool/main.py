import customtkinter
import serial
import serial.tools.list_ports
import time
import os
import shutil
from tkinter import filedialog
import json
import sys

class App(customtkinter.CTk):
    def __init__(self):
        super().__init__()

        self.title("Buttonbox Config Tool")
        self.geometry("900x600")
        customtkinter.set_appearance_mode("System")
        customtkinter.set_default_color_theme("blue")

        # Serial Connection Manager
        self.serial_connection = None
        self.firmware_path = None
        self.config_data = {} # Initialize to store parsed JSON config
        self.listening_mode = False

        # Layout: Sidebar (Left) for connection, Main (Right) for content
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)
        
        self.after(100, self.read_serial_loop)

        self.sidebar_frame = customtkinter.CTkFrame(self, width=200, corner_radius=0)
        self.sidebar_frame.grid(row=0, column=0, sticky="nsew")
        
        self.main_frame = customtkinter.CTkFrame(self, corner_radius=0)
        self.main_frame.grid(row=0, column=1, sticky="nsew")

        # --- Sidebar Content ---
        self.logo_label = customtkinter.CTkLabel(self.sidebar_frame, text="Buttonbox Config", font=customtkinter.CTkFont(size=20, weight="bold"))
        self.logo_label.grid(row=0, column=0, padx=20, pady=(20, 10))

        # Port Selection
        self.port_label = customtkinter.CTkLabel(self.sidebar_frame, text="Select Port:", anchor="w")
        self.port_label.grid(row=1, column=0, padx=20, pady=(10, 0))

        self.port_option_menu = customtkinter.CTkOptionMenu(self.sidebar_frame, values=self.get_serial_ports())
        self.port_option_menu.grid(row=2, column=0, padx=20, pady=(10, 10))

        # Refresh Button
        self.refresh_button = customtkinter.CTkButton(self.sidebar_frame, text="Refresh Ports", command=self.refresh_ports)
        self.refresh_button.grid(row=3, column=0, padx=20, pady=10)

        # Connect Button
        self.connect_button = customtkinter.CTkButton(self.sidebar_frame, text="Connect", command=self.toggle_connection, fg_color="green")
        self.connect_button.grid(row=4, column=0, padx=20, pady=10)

        self.status_label = customtkinter.CTkLabel(self.sidebar_frame, text="Status: Disconnected", text_color="gray", wraplength=180)
        self.status_label.grid(row=5, column=0, padx=20, pady=(10, 20))

        # --- Main Frame Content (Tabs) ---
        self.tab_view = customtkinter.CTkTabview(self.main_frame)
        self.tab_view.pack(padx=20, pady=20, fill="both", expand=True)

        self.tab_config = self.tab_view.add("Configuration")
        self.tab_firmware = self.tab_view.add("Firmware Update")

        self.setup_firmware_tab()
        self.setup_config_tab()

    def setup_config_tab(self):
        # --- Generator Section ---
        self.gen_frame = customtkinter.CTkFrame(self.tab_config)
        self.gen_frame.pack(pady=10, padx=10, fill="x")
        
        self.gen_label = customtkinter.CTkLabel(self.gen_frame, text="Create New Config", font=customtkinter.CTkFont(weight="bold"))
        self.gen_label.grid(row=0, column=0, columnspan=4, pady=5)

        # Dropdowns
        self.lbl_funky = customtkinter.CTkLabel(self.gen_frame, text="Funkys:")
        self.lbl_funky.grid(row=1, column=0, padx=5)
        self.combo_funky = customtkinter.CTkOptionMenu(self.gen_frame, values=["0", "1", "2"], width=70)
        self.combo_funky.grid(row=1, column=1, padx=5)

        self.lbl_enc = customtkinter.CTkLabel(self.gen_frame, text="Encoders:")
        self.lbl_enc.grid(row=1, column=2, padx=5)
        self.combo_enc = customtkinter.CTkOptionMenu(self.gen_frame, values=["0", "1", "2", "3", "4", "5"], width=70)
        self.combo_enc.grid(row=1, column=3, padx=5)

        self.lbl_btn = customtkinter.CTkLabel(self.gen_frame, text="Buttons:")
        self.lbl_btn.grid(row=1, column=4, padx=5)
        self.combo_btn = customtkinter.CTkOptionMenu(self.gen_frame, values=[str(i) for i in range(9)], width=70) # 0-8
        self.combo_btn.grid(row=1, column=5, padx=5)

        self.btn_generate = customtkinter.CTkButton(self.gen_frame, text="Generate Empty Config", command=self.generate_config, fg_color="teal")
        self.btn_generate.grid(row=2, column=0, columnspan=6, pady=10)

        # --- Detection Section ---
        self.det_frame = customtkinter.CTkFrame(self.tab_config)
        self.det_frame.pack(pady=10, padx=10, fill="x")
        
        self.det_label = customtkinter.CTkLabel(self.det_frame, text="Input Detector / Latching Switch Config", font=customtkinter.CTkFont(weight="bold"))
        self.det_label.pack(pady=5)

        self.det_status_label = customtkinter.CTkLabel(self.det_frame, text="Status: Idle. Connect and press 'Start Listening'.", text_color="gray")
        self.det_status_label.pack(pady=2)

        self.det_btn_frame = customtkinter.CTkFrame(self.det_frame, fg_color="transparent")
        self.det_btn_frame.pack(pady=5)

        self.btn_listen = customtkinter.CTkButton(self.det_btn_frame, text="Start Listening", command=self.toggle_listening, width=120)
        self.btn_listen.grid(row=0, column=0, padx=5)
        
        self.btn_add_latching = customtkinter.CTkButton(self.det_btn_frame, text="Add as Latching Switch", command=self.add_detected_latching, state="disabled", fg_color="gray", width=150)
        self.btn_add_latching.grid(row=0, column=1, padx=5)

        self.chk_auto_g = customtkinter.CTkCheckBox(self.det_btn_frame, text="Auto-Assign G-Buttons")
        self.chk_auto_g.grid(row=0, column=2, padx=5)
        self.chk_auto_g.select()

        self.last_detected_event = None

        # --- Existing Load/Save Section ---
        self.config_label = customtkinter.CTkLabel(self.tab_config, text="Current Configuration", font=customtkinter.CTkFont(size=16, weight="bold"))
        self.config_label.pack(pady=(20, 5))

        self.load_config_btn = customtkinter.CTkButton(self.tab_config, text="Load Config from Device", command=self.load_config)
        self.load_config_btn.pack(pady=5)

        self.load_file_btn = customtkinter.CTkButton(self.tab_config, text="Load Config from File", command=self.load_config_from_file)
        self.load_file_btn.pack(pady=5)

        # Frame for dynamic configuration display
        self.config_display_frame = customtkinter.CTkScrollableFrame(self.tab_config, width=600, height=250)
        self.config_display_frame.pack(pady=10, fill="both", expand=True)

        self.config_textbox = customtkinter.CTkTextbox(self.tab_config, width=600, height=100) # Reduced size for raw view
        self.config_textbox.pack(pady=5)

        self.save_config_btn = customtkinter.CTkButton(self.tab_config, text="Save Config to Device", command=self.save_config, fg_color="green")
        self.save_config_btn.pack(pady=10)

    def generate_config(self):
        try:
            n_funky = int(self.combo_funky.get())
            n_enc = int(self.combo_enc.get())
            n_btn = int(self.combo_btn.get())

            new_config = {
                "version": 1.1,
                "entities": {},
                "pages": [{}]
            }

            # Hardware ID Mapping (Based on Firmware Analysis)
            # Funky 1=0, Funky 2=1
            # Encoders start at 2
            # ShiftRegister (Buttons) is 7

            # 1. Add Funkys
            for i in range(n_funky):
                name = f"FunkySwitch{i+1}"
                new_config["entities"][name] = i
                # Add default structure to Page 0
                new_config["pages"][0][name] = {
                    "Up": {"press": []}, "Down": {"press": []}, 
                    "Left": {"press": []}, "Right": {"press": []}, 
                    "Center": {"press": []}, 
                    "Encoder": {"increase": [], "decrease": []}
                }

            # 2. Add Encoders
            enc_start_id = 2
            for i in range(n_enc):
                name = f"Encoder{i+1}"
                new_config["entities"][name] = enc_start_id + i
                # Add default structure
                new_config["pages"][0][name] = {
                    "increase": [], "decrease": []
                }

            # 3. Add Buttons (Shift Register)
            if n_btn > 0:
                name = "ShiftRegButtons"
                new_config["entities"][name] = 7 # Fixed ID for ShiftRegister
                new_config["pages"][0][name] = {}
                for i in range(n_btn):
                    # Sub-entities for shift register
                    new_config["pages"][0][name][str(i)] = {
                        "press": [], "release": []
                    }

            self.config_data = new_config
            
            # Update UI
            self.config_textbox.delete("0.0", "end")
            self.config_textbox.insert("0.0", json.dumps(self.config_data, indent=2))
            self.display_config_data()
            self.status_label.configure(text="New config generated!", text_color="blue")

        except Exception as e:
            self.status_label.configure(text=f"Error generating config: {e}", text_color="red")

    def load_config(self):
        if self.serial_connection and self.serial_connection.is_open:
            try:
                self.serial_connection.reset_input_buffer()
                self.serial_connection.write(b"__PRINT_CONFIG__\n")
                
                # Read response until a valid JSON object is received or timeout
                received_data = ""
                start_time = time.time()
                while time.time() - start_time < 2:  # 2 second timeout for reading
                    if self.serial_connection.in_waiting:
                        chunk = self.serial_connection.read(self.serial_connection.in_waiting).decode('utf-8', errors='ignore')
                        received_data += chunk
                        if received_data.strip().endswith('}'): # Simple check for end of JSON
                            break
                    time.sleep(0.01) # Small delay to prevent busy-waiting

                if received_data:
                    try:
                        self.config_data = json.loads(received_data)
                        self.config_textbox.delete("0.0", "end")
                        self.config_textbox.insert("0.0", json.dumps(self.config_data, indent=2))
                        self.status_label.configure(text="Config loaded and parsed.", text_color="green")
                        self.display_config_data() # Call new method to display structured data
                    except json.JSONDecodeError as e:
                        self.config_data = {} # Clear potentially invalid data
                        self.config_textbox.delete("0.0", "end")
                        self.config_textbox.insert("0.0", f"Error parsing JSON: {e}\n\nReceived raw data:\n{received_data}")
                        self.status_label.configure(text=f"Error parsing config: {e}", text_color="red")
                else:
                    self.config_data = {}
                    self.config_textbox.delete("0.0", "end")
                    self.config_textbox.insert("0.0", "No data received from device.")
                    self.status_label.configure(text="No response received.", text_color="orange")

            except Exception as e:
                self.config_data = {}
                self.status_label.configure(text=f"Error loading config: {e}", text_color="red")
        else:
            # MOCK MODE for testing without hardware
            mock_json_str = '{\n  "mock": "data",\n  "buttons": [1, 2, 3],\n  "nested": {"key": "value"}\n}'
            try:
                self.config_data = json.loads(mock_json_str)
                self.config_textbox.delete("0.0", "end")
                self.config_textbox.insert("0.0", json.dumps(self.config_data, indent=2))
                self.status_label.configure(text="Mock Config loaded and parsed (Not connected).", text_color="blue")
            except json.JSONDecodeError as e:
                self.config_data = {}
                self.config_textbox.delete("0.0", "end")
                self.config_textbox.insert("0.0", f"Error parsing mock JSON: {e}")
                self.status_label.configure(text=f"Error parsing mock config: {e}", text_color="red")

    def save_config(self):
        if not (self.serial_connection and self.serial_connection.is_open):
            self.status_label.configure(text="Error: Not connected to device.", text_color="red")
            return
        if not self.config_data:
            self.status_label.configure(text="Error: No configuration loaded to save.", text_color="red")
            return

        try:
            config_json_str = json.dumps(self.config_data, indent=2)
            filename = "gui_config.json" # Fixed filename for simplicity

            self.serial_connection.reset_input_buffer()

            # Send protocol commands
            self.serial_connection.write(b"__CHANGE_CONFIG_FILE__\n")
            time.sleep(0.05)
            self.serial_connection.write(f"{filename}\n".encode('utf-8'))
            time.sleep(0.05)
            self.serial_connection.write(b"__CHANGE_DEFAULT_CONFIG__\n")
            time.sleep(0.05)
            self.serial_connection.write(f"{filename}\n".encode('utf-8'))
            time.sleep(0.05)

            self.serial_connection.write(b"__NEW_CONFIG__\n")
            time.sleep(0.05)
            self.serial_connection.write(b"__START__\n")
            time.sleep(0.05)
            
            # Send config data in chunks to prevent buffer overflow on device
            chunk_size = 64 # Adjust as needed for device buffer
            for i in range(0, len(config_json_str), chunk_size):
                chunk = config_json_str[i:i + chunk_size].encode('utf-8')
                self.serial_connection.write(chunk)
                time.sleep(0.001) # Small delay between chunks

            self.serial_connection.write(b"\n") # Ensure newline at the end
            time.sleep(0.05)
            self.serial_connection.write(b"__END__\n")
            time.sleep(0.05)
            self.serial_connection.write(b"__RELOAD__\n")
            time.sleep(0.5) # Give device time to process and reload

            # Read device response for confirmation (optional, but good practice)
            response = ""
            if self.serial_connection.in_waiting:
                response = self.serial_connection.read(self.serial_connection.in_waiting).decode('utf-8', errors='ignore')
            
            if "Config saved and reloaded" in response or "OK" in response: # Assuming device sends a confirmation
                self.status_label.configure(text=f"Config '{filename}' saved and reloaded successfully!", text_color="green")
            else:
                self.status_label.configure(text=f"Config saved, but no clear confirmation from device. Response: {response}", text_color="orange")

        except Exception as e:
            self.status_label.configure(text=f"Error saving config: {e}", text_color="red")

    def display_config_data(self):
        # Clear existing widgets
        for widget in self.config_display_frame.winfo_children():
            widget.destroy()

        if not self.config_data:
            customtkinter.CTkLabel(self.config_display_frame, text="No configuration data loaded.").pack(pady=5)
            return

        row_counter = 0
        for key, value in self.config_data.items():
            # Add section title
            section_title = customtkinter.CTkLabel(self.config_display_frame, text=f"--- {key.capitalize()} ---", font=customtkinter.CTkFont(size=14, weight="bold"))
            section_title.grid(row=row_counter, column=0, sticky="w", padx=10, pady=(10, 0))
            row_counter += 1

            if key == "pages":
                for i, page in enumerate(value):
                    page_label = customtkinter.CTkLabel(self.config_display_frame, text=f"  Page {i}:")
                    page_label.grid(row=row_counter, column=0, sticky="w", padx=20)
                    row_counter += 1
                    # Iterate through entities in page, e.g., FunkySwitch, KeyMatrix
                    for entity_name, entity_config in page.items():
                        entity_label = customtkinter.CTkLabel(self.config_display_frame, text=f"    {entity_name}: {len(entity_config)} entries")
                        entity_label.grid(row=row_counter, column=0, sticky="w", padx=30)
                        row_counter += 1
            elif key == "templates":
                for template_name, template_data in value.items():
                    actions_count = len(template_data.get("actions", []))
                    template_label = customtkinter.CTkLabel(self.config_display_frame, text=f"  Template '{template_name}': {actions_count} actions")
                    template_label.grid(row=row_counter, column=0, sticky="w", padx=20)
                    row_counter += 1
            elif key == "entities" or key == "sub_entities":
                for entity_key, entity_value in value.items():
                    if isinstance(entity_value, dict):
                         sub_entity_count = len(entity_value)
                         entity_label = customtkinter.CTkLabel(self.config_display_frame, text=f"  {entity_key}: {sub_entity_count} sub-entities")
                    else:
                        entity_label = customtkinter.CTkLabel(self.config_display_frame, text=f"  {entity_key}: {entity_value}")
                    entity_label.grid(row=row_counter, column=0, sticky="w", padx=20)
                    row_counter += 1
            else:
                # For other simple key-value pairs
                item_label = customtkinter.CTkLabel(self.config_display_frame, text=f"  {key}: {value}")
                item_label.grid(row=row_counter, column=0, sticky="w", padx=20)
                row_counter += 1

    def setup_firmware_tab(self):
        self.fw_label = customtkinter.CTkLabel(self.tab_firmware, text="Firmware Update", font=customtkinter.CTkFont(size=16, weight="bold"))
        self.fw_label.pack(pady=10)

        self.bootloader_btn = customtkinter.CTkButton(self.tab_firmware, text="1. Reboot to Bootloader", command=self.reboot_bootloader)
        self.bootloader_btn.pack(pady=10)

        # Firmware Source Selection
        self.fw_source_var = customtkinter.StringVar(value="auto")
        self.radio_frame = customtkinter.CTkFrame(self.tab_firmware, fg_color="transparent")
        self.radio_frame.pack(pady=5)
        self.radio_auto = customtkinter.CTkRadioButton(self.radio_frame, text="Pre-compiled", variable=self.fw_source_var, value="auto", command=self.update_fw_ui)
        self.radio_auto.grid(row=0, column=0, padx=10)
        self.radio_manual = customtkinter.CTkRadioButton(self.radio_frame, text="Manual File", variable=self.fw_source_var, value="manual", command=self.update_fw_ui)
        self.radio_manual.grid(row=0, column=1, padx=10)

        # Auto Selection UI
        self.auto_frame = customtkinter.CTkFrame(self.tab_firmware)
        self.auto_frame.pack(pady=10)
        self.lbl_variant = customtkinter.CTkLabel(self.auto_frame, text="Select Device:")
        self.lbl_variant.grid(row=0, column=0, padx=10, pady=10)
        
        # Map friendly names to filenames
        self.firmware_map = {
            "Buttonbox (2 Funkys)": "buttonbox_2funky.uf2",
            "Minikeyboard": "minikeyboard.uf2",
            "Buttonbox (1 Funky)": "buttonbox_1funky.uf2"
        }
        self.combo_variant = customtkinter.CTkOptionMenu(self.auto_frame, values=list(self.firmware_map.keys()))
        self.combo_variant.grid(row=0, column=1, padx=10, pady=10)

        # Manual Selection UI
        self.manual_frame = customtkinter.CTkFrame(self.tab_firmware)
        self.select_file_btn = customtkinter.CTkButton(self.manual_frame, text="2. Select .uf2 File", command=self.select_firmware_file)
        self.select_file_btn.pack(pady=10)
        self.selected_file_label = customtkinter.CTkLabel(self.manual_frame, text="No file selected", text_color="gray")
        self.selected_file_label.pack(pady=5)

        self.flash_btn = customtkinter.CTkButton(self.tab_firmware, text="3. Flash Firmware", command=self.flash_firmware, fg_color="orange")
        self.flash_btn.pack(pady=20)
        
        self.update_fw_ui()

    def update_fw_ui(self):
        if self.fw_source_var.get() == "auto":
            self.manual_frame.pack_forget()
            self.auto_frame.pack(pady=10)
        else:
            self.auto_frame.pack_forget()
            self.manual_frame.pack(pady=10)

    def get_resource_path(self, relative_path):
        """ Get absolute path to resource, works for dev and for PyInstaller """
        try:
            # PyInstaller creates a temp folder and stores path in _MEIPASS
            base_path = sys._MEIPASS
        except Exception:
            base_path = os.path.dirname(os.path.abspath(__file__))

        return os.path.join(base_path, relative_path)

    def select_firmware_file(self):
        filename = filedialog.askopenfilename(filetypes=[("UF2 Files", "*.uf2")])
        if filename:
            self.firmware_path = filename
            self.selected_file_label.configure(text=os.path.basename(filename))

    def flash_firmware(self):
        target_firmware = None

        if self.fw_source_var.get() == "auto":
            variant = self.combo_variant.get()
            filename = self.firmware_map.get(variant)
            if filename:
                # Look in 'firmware' subdir
                target_firmware = self.get_resource_path(os.path.join("firmware", filename))
                if not os.path.exists(target_firmware):
                     self.status_label.configure(text=f"Error: Firmware file not found at {target_firmware}", text_color="red")
                     return
        else:
            target_firmware = self.firmware_path

        if not target_firmware:
            self.status_label.configure(text="Error: No firmware selected.", text_color="red")
            return

        # Find RPI-RP2 drive
        drive_path = self.find_pico_drive()
        
        if drive_path:
            try:
                self.status_label.configure(text=f"Flashing {os.path.basename(target_firmware)}...")
                self.update() # Update UI
                shutil.copy2(target_firmware, drive_path)
                self.status_label.configure(text="Flashing complete! Device is rebooting.", text_color="green")
            except Exception as e:
                self.status_label.configure(text=f"Error flashing: {e}", text_color="red")
        else:
             self.status_label.configure(text="Error: RPI-RP2 drive not found. Did you press 'Reboot to Bootloader'?", text_color="red")

    def get_serial_ports(self):
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports] if ports else ["No Ports Found"]

    def refresh_ports(self):
        ports = self.get_serial_ports()
        self.port_option_menu.configure(values=ports)
        self.port_option_menu.set(ports[0] if ports else "No Ports Found")

    def toggle_connection(self):
        if self.serial_connection and self.serial_connection.is_open:
            # Disconnect
            self.serial_connection.close()
            self.serial_connection = None
            self.connect_button.configure(text="Connect", fg_color="green")
            self.status_label.configure(text="Status: Disconnected", text_color="gray")
            self.port_option_menu.configure(state="normal")
            self.refresh_button.configure(state="normal")
        else:
            # Connect
            selected_port = self.port_option_menu.get()
            if selected_port == "No Ports Found":
                return
            
            try:
                self.serial_connection = serial.Serial(selected_port, 115200, timeout=1)
                self.connect_button.configure(text="Disconnect", fg_color="red")
                self.status_label.configure(text="Status: Connected", text_color="green")
                self.port_option_menu.configure(state="disabled")
                self.refresh_button.configure(state="disabled")
            except serial.SerialException as e:
                self.status_label.configure(text=f"Error: {e}", text_color="red")

    def reboot_bootloader(self):
        if self.serial_connection and self.serial_connection.is_open:
            try:
                self.serial_connection.write(b"__BOOTLOADER__\n")
                self.status_label.configure(text="Sent bootloader command. Device should reboot as storage.")
                # Close serial connection as device will reboot
                self.toggle_connection() 
            except Exception as e:
                self.status_label.configure(text=f"Error sending command: {e}", text_color="red")
        else:
            self.status_label.configure(text="Error: Not connected", text_color="red")

    def find_pico_drive(self):
        # Simple heuristic to find the Pico drive
        # Linux: /media/<user>/RPI-RP2 or /run/media/<user>/RPI-RP2
        # Windows: Iterate drives and check label (harder in Python without win32api)
        
        # Linux check
        username = os.environ.get('USER')
        if username:
            paths = [
                f"/media/{username}/RPI-RP2",
                f"/run/media/{username}/RPI-RP2"
            ]
            for path in paths:
                if os.path.exists(path):
                    return path
            
        # Check common mount points (e.g. /mnt, /media)
        for base_path in ["/media", "/run/media"]:
            if os.path.exists(base_path):
                try:
                    for user in os.listdir(base_path):
                        pico_path = os.path.join(base_path, user, "RPI-RP2")
                        if os.path.exists(pico_path):
                            return pico_path
                except OSError:
                    continue

        # Fallback: Ask user to select the drive
        drive = filedialog.askdirectory(title="Select RPI-RP2 Drive")
        if drive:
            return drive
            
        return None

    def load_config_from_file(self):
        file_path = filedialog.askopenfilename(
            title="Select Configuration File",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        if file_path:
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    file_content = f.read()
                    self.config_data = json.loads(file_content)
                    self.config_textbox.delete("0.0", "end")
                    self.config_textbox.insert("0.0", json.dumps(self.config_data, indent=2))
                    self.display_config_data() # Update the structured display if it exists
                    self.status_label.configure(text=f"Config loaded from {os.path.basename(file_path)}", text_color="green")
            except json.JSONDecodeError as e:
                self.status_label.configure(text=f"Error parsing JSON from file: {e}", text_color="red")
            except Exception as e:
                self.status_label.configure(text=f"Error reading file: {e}", text_color="red")

    def toggle_listening(self):
        if not (self.serial_connection and self.serial_connection.is_open):
             self.status_label.configure(text="Error: Not connected.", text_color="red")
             return
        
        self.listening_mode = not self.listening_mode
        if self.listening_mode:
            self.btn_listen.configure(text="Stop Listening", fg_color="red")
            self.det_status_label.configure(text="Listening... Press a button on device.", text_color="blue")
        else:
            self.btn_listen.configure(text="Start Listening", fg_color="#1f6aa5") # Default blueish
            self.det_status_label.configure(text="Stopped listening.", text_color="gray")

    def read_serial_loop(self):
        if self.listening_mode and self.serial_connection and self.serial_connection.is_open:
            try:
                if self.serial_connection.in_waiting:
                    line = self.serial_connection.readline().decode('utf-8', errors='ignore').strip()
                    if line.startswith("__INPUT_EVENT__:"):
                        # Format: __INPUT_EVENT__:entity:subId:value
                        parts = line.split(":")
                        if len(parts) == 4:
                            entity = int(parts[1])
                            sub_id = int(parts[2])
                            val = int(parts[3])
                            
                            # Resolve names if possible (reverse lookup in self.config_data if available)
                            entity_name = str(entity)
                            sub_name = str(sub_id)
                            
                            # Try to find names in loaded config
                            if "entities" in self.config_data:
                                for name, id_val in self.config_data["entities"].items():
                                    if id_val == entity:
                                        entity_name = name
                                        break
                            
                            # Display detection
                            self.last_detected_event = (entity_name, sub_name, entity, sub_id) # Store mixed name/id
                            
                            status_text = f"Detected: {entity_name} -> Sub: {sub_name} (Value: {val})"
                            self.det_status_label.configure(text=status_text, text_color="green")
                            
                            # Enable "Add Latching" button
                            self.btn_add_latching.configure(state="normal", fg_color="green")

            except Exception as e:
                print(f"Serial read error: {e}")
        
        self.after(50, self.read_serial_loop) # Call again

    def get_next_free_gamepad_buttons(self, count=1):
        used_ids = set()
        # Scan config for "G0"-"G31"
        # Simple regex on the string dump is efficient enough here
        config_str = json.dumps(self.config_data)
        import re
        matches = re.findall(r'"G(\d+)"', config_str)
        for m in matches:
            used_ids.add(int(m))
            
        free_buttons = []
        candidate = 0
        while len(free_buttons) < count and candidate < 128: 
            if candidate not in used_ids:
                free_buttons.append(f"G{candidate}")
            candidate += 1
        return free_buttons

    def add_detected_latching(self):
        if not self.last_detected_event:
            return
            
        entity_name, sub_name, entity_id, sub_id = self.last_detected_event
        
        # Ensure config structure exists
        if not self.config_data:
             self.status_label.configure(text="Please generate or load a config first.", text_color="red")
             return

        if "pages" not in self.config_data or not self.config_data["pages"]:
            self.config_data["pages"] = [{}]
            
        page = self.config_data["pages"][0]
        
        if entity_name not in page:
            page[entity_name] = {}

        key_on = None
        key_off = None

        if self.chk_auto_g.get():
            # Auto Assign
            g_buttons = self.get_next_free_gamepad_buttons(2)
            if len(g_buttons) == 2:
                key_on = g_buttons[0]
                key_off = g_buttons[1]
            else:
                 self.status_label.configure(text="Error: Not enough free G-Buttons found.", text_color="red")
                 return
        else:
            # Ask user for Keys to map
            key_on = customtkinter.CTkInputDialog(text=f"Enter Key for ON (Switch Close) for {entity_name}:{sub_name}:", title="Latching ON").get_input()
            if not key_on: return
            
            key_off = customtkinter.CTkInputDialog(text=f"Enter Key for OFF (Switch Open) for {entity_name}:{sub_name}:", title="Latching OFF").get_input()
            if not key_off: return

        # Create/Overwrite entry
        # Latching logic: Press (Close) -> Push Key A; Release (Open) -> Push Key B
        page[entity_name][str(sub_name)] = {
            "press": [{"push": key_on}],
            "release": [{"push": key_off}]
        }
        
        # Update UI
        self.config_textbox.delete("0.0", "end")
        self.config_textbox.insert("0.0", json.dumps(self.config_data, indent=2))
        self.display_config_data()
        self.status_label.configure(text=f"Added Latching: {entity_name}:{sub_name} -> ON:{key_on}, OFF:{key_off}", text_color="green")

if __name__ == "__main__":
    app = App()
    app.mainloop()
