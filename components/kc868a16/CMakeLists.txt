idf_component_register(
    SRCS "KC868A16.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        i2c_manager
        pcf8574
)
