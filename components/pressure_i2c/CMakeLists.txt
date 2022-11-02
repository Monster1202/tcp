set(srcs "src/pressure_i2c.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES para_list gpio_ctrl)
