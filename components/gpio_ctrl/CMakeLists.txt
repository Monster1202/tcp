set(srcs "src/gpio_ctrl.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES para_list timer_app mqtt_app)
