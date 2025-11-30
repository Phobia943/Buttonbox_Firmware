import customtkinter
import serial
import serial.tools.list_ports
import time
import os
import shutil
from tkinter import filedialog

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

        # Layout: Sidebar (Left) for connection, Main (Right) for content
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

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
        self.config_label = customtkinter.CTkLabel(self.tab_config, text="Device Configuration", font=customtkinter.CTkFont(size=16, weight="bold"))
        self.config_label.pack(pady=10)

        self.load_config_btn = customtkinter.CTkButton(self.tab_config, text="Load Config from Device", command=self.load_config)
        self.load_config_btn.pack(pady=10)

        self.config_textbox = customtkinter.CTkTextbox(self.tab_config, width=600, height=400)
        self.config_textbox.pack(pady=10)

        self.save_config_btn = customtkinter.CTkButton(self.tab_config, text="Save Config to Device", command=self.save_config, fg_color="green")
        self.save_config_btn.pack(pady=10)

    def load_config(self):
        if self.serial_connection and self.serial_connection.is_open:
            try:
                # Clear buffer
                self.serial_connection.reset_input_buffer()
                
                # Send command
                self.serial_connection.write(b"__PRINT_CONFIG__\n")
                
                # Read response (simple implementation with timeout)
                # In reality, we should read line by line until we get a valid JSON or end marker
                time.sleep(0.5) # Wait for data
                if self.serial_connection.in_waiting:
                    data = self.serial_connection.read(self.serial_connection.in_waiting).decode('utf-8', errors='ignore')
                    self.config_textbox.delete("0.0", "end")
                    self.config_textbox.insert("0.0", data)
                    self.status_label.configure(text="Config loaded.")
                else:
                     self.status_label.configure(text="No response received.", text_color="orange")

            except Exception as e:
                self.status_label.configure(text=f"Error loading config: {e}", text_color="red")
        else:
             # MOCK MODE for testing without hardware
             self.config_textbox.delete("0.0", "end")
             self.config_textbox.insert("0.0", '{\n  "mock": "data",\n  "buttons": [1, 2, 3]\n}')
             self.status_label.configure(text="Mock Config loaded (Not connected).")

    def save_config(self):
         # TODO: Implement sending config back (__NEW_CONFIG__ protocol)
         self.status_label.configure(text="Save Config not implemented yet.")

    def setup_firmware_tab(self):
        self.fw_label = customtkinter.CTkLabel(self.tab_firmware, text="Firmware Update", font=customtkinter.CTkFont(size=16, weight="bold"))
        self.fw_label.pack(pady=20)

        self.bootloader_btn = customtkinter.CTkButton(self.tab_firmware, text="1. Reboot to Bootloader", command=self.reboot_bootloader)
        self.bootloader_btn.pack(pady=10)
        
        self.select_file_btn = customtkinter.CTkButton(self.tab_firmware, text="2. Select .uf2 File", command=self.select_firmware_file)
        self.select_file_btn.pack(pady=10)
        
        self.selected_file_label = customtkinter.CTkLabel(self.tab_firmware, text="No file selected", text_color="gray")
        self.selected_file_label.pack(pady=5)

        self.flash_btn = customtkinter.CTkButton(self.tab_firmware, text="3. Flash Firmware", command=self.flash_firmware, state="disabled", fg_color="orange")
        self.flash_btn.pack(pady=20)

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

    def select_firmware_file(self):
        filename = filedialog.askopenfilename(filetypes=[("UF2 Files", "*.uf2")])
        if filename:
            self.firmware_path = filename
            self.selected_file_label.configure(text=os.path.basename(filename))
            self.flash_btn.configure(state="normal")

    def flash_firmware(self):
        if not self.firmware_path:
            return

        # Find RPI-RP2 drive
        drive_path = self.find_pico_drive()
        
        if drive_path:
            try:
                self.status_label.configure(text=f"Flashing to {drive_path}...")
                self.update() # Update UI
                shutil.copy2(self.firmware_path, drive_path)
                self.status_label.configure(text="Flashing complete! Device is rebooting.", text_color="green")
            except Exception as e:
                self.status_label.configure(text=f"Error flashing: {e}", text_color="red")
        else:
             self.status_label.configure(text="Error: RPI-RP2 drive not found. Did you press 'Reboot to Bootloader'?", text_color="red")

    def find_pico_drive(self):
        # Simple heuristic to find the Pico drive
        # Linux: /media/<user>/RPI-RP2
        # Windows: Iterate drives and check label (harder in Python without win32api)
        
        # Linux check
        username = os.environ.get('USER')
        if username:
            linux_path = f"/media/{username}/RPI-RP2"
            if os.path.exists(linux_path):
                return linux_path
            
        # Check common mount points (e.g. /mnt, /media)
        if os.path.exists("/media"):
             for user in os.listdir("/media"):
                 pico_path = os.path.join("/media", user, "RPI-RP2")
                 if os.path.exists(pico_path):
                     return pico_path

        # Fallback: Ask user to select the drive
        drive = filedialog.askdirectory(title="Select RPI-RP2 Drive")
        if drive and "RPI-RP2" in drive: # Weak check, but better than nothing
            return drive
            
        return None

if __name__ == "__main__":
    app = App()
    app.mainloop()
