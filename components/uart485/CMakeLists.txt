set(srcs "src/uart485.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include")
