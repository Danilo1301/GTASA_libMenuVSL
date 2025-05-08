# MenuVSL

<h1>Required</h1>

* libAML.so (https://github.com/RusJJ/AndroidModLoader)
* libCLEOMod.so (https://github.com/AndroidModLoader/GTA_CLEOMod)
* libSAUtils.so (optional) (https://github.com/AndroidModLoader/SAUtils)
    
<h1>How to install</h1>

1. Download the **MenuVSL-0.0.0.zip** file in [Releases](https://github.com/Danilo1301/GTASA_libMenuVSL/releases)
2. Copy the folder 'com.rockstargames.gtasa' and paste it in your /data/ folder (ex Android/data)

<h1>How to test</h1>

1. Open the cleo menu, and select the Test Menu VSL.csi

2. You can also run the cleo script: Menu VSL.csi

<h1>How to use</h1>

### Using CLEO opcodes:

1. Open your Sanny Builder folder, go to 'data\sa_mobile\SASCM.ini'

2. Add this text to the end of the file:

```
; MenuVSL

4D70=4,%4d% = menu_vsl_add_window title %1g% x %2d% y %3d%
4D71=3,%3d% = menu_vsl_test %1d% %2g%
4D72=3,%3d% = menu_vsl_add_text window %1d% text %2g%
4D73=3,%3d% = menu_vsl_add_button window %1d% text %2g%
4D74=1,menu_vsl_is_button_just_pressed %1d%
4D75=1,menu_vsl_remove_window %1d%
4D76=7,%7d% = menu_vsl_add_int_range window %1d% text %2g% value %3d% min %4d% max %5d% add_by %6d%
4D77=7,%7d% = menu_vsl_add_float_range window %1d% text %2g% value %3d% min %4d% max %5d% add_by %6d%
4D78=2,%2g% = menu_vsl_get_int_value item %1d%
4D79=2,%2g% = menu_vsl_get_float_value item %1d%
4D7A=2,menu_vsl_set_hold_to_change item %1d% value %2d%
4D7B=1,menu_vsl_set_debug_visible %1d%
4D7C=2,menu_vsl_add_debug_int %1g% %2d%
4D7D=2,menu_vsl_add_debug_float %1g% %2d%
4D7E=4,%4d% = menu_vsl_add_checkbox window %1d% text %2g% value %3d%
4D7F=5,menu_vsl_set_window_background window %1d% r %2d% g %3d% b %4d% a %5d%
4D80=2,set_global_int_variable %1g% value %2d%
4D81=2,%2d% = get_global_int_variable %1g%
```

3. Create a CLEO Script for GTA SA Android

4. Create a window using the 4D70 opcode:

```ini
4D70: $Window1 = menu_vsl_add_window title "Nice window" x 450 y 200
```

5. Create a button using the 4D73 opcode:

```ini
4D73: $Button1 = menu_vsl_add_button window $Window1 text "~w~> ~r~Close"
```

6. Now you can compile and test it

More examples here: [cleo](https://github.com/Danilo1301/GTASA_libMenuVSL/tree/main/cleo)

### Using C++

You can ask me on Discord if you need

<h1>Discord</h1>

* https://discord.gg/mkCDRf4zJA