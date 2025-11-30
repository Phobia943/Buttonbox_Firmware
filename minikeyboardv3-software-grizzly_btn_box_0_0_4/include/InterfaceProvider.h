//
// Created by Simon on 11.02.2024.
//
#pragma once
#include <string>

class InterfaceProvider {
public:
    typedef enum{
        ResultNothing,
        ResultChangeConfig,
        ResultChangeDefaultConfig,
        ResultReadNewConfig,
        ResultReload,
        ResultPrintCurrentConfig,
    }interfaceUpdateResult_t;

protected:
    InterfaceProvider() = default;
    bool connected = false;
    std::string lastConfFileName;

public:
    virtual ~InterfaceProvider() = default;
    virtual interfaceUpdateResult_t update() = 0;
    virtual bool readLine(std::string& line) = 0;
    virtual std::string getNewConfigName() = 0;

    [[nodiscard]] bool isConnected() const { return connected; }
    [[nodiscard]] std::string getLastConfFileName() const { return lastConfFileName; }
};

class SerialInterfaceProvider : public InterfaceProvider{
private:
    std::string inputBuffer;
    void processSimHubCommand(const std::string& cmd);

public:
    SerialInterfaceProvider();
    ~SerialInterfaceProvider() override = default;

    interfaceUpdateResult_t update() override;
    bool readLine(std::string& line) override;
    std::string getNewConfigName() override;
};

