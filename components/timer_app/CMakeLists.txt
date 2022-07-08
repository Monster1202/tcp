set(srcs "src/timer_app.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES para_list gpio_ctrl)
