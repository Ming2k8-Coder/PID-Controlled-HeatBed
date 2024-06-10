idf_component_register(
    SRCS "tempitf.cpp" "pidalgo.cpp"
    "manual.cpp" 
    "menu.cpp" 
    "rotary_lib/rotary_encoder.c" 
         "main.cpp"
         "lcd.cpp"
         "triac_zcd.cpp"
         "dimmer/dimmer.c"
        "adcitf.cpp"
        
         "tempitf"

    INCLUDE_DIRS "rotary_lib" "dimmer"
     
)
