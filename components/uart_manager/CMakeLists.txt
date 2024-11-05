idf_component_register(
    SRCS "UartManager.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        driver
        freertos
)
