set(applicationSourceCode
    Src/periphery/pwm/pwm.cpp
    Src/periphery/led/led.cpp

    Src/cyphal_application/setpoint/setpoint.cpp
    Src/cyphal_application/feedback/feedback.cpp
    Src/cyphal_application/application.cpp
)
set(applicationHeaders
    Src
    Src/cyphal_application
    Libs/Cyphal
)

list(APPEND cyphalRegisters
    ${CMAKE_CURRENT_LIST_DIR}/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/setpoint/params.yaml
    ${CMAKE_CURRENT_LIST_DIR}/feedback/params.yaml
)
