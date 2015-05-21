// qed.h header file for qed.c

// if defined an instiutional version 
// ownername is taken out of QED's code
// #define INSTITUTIONAL           1

// normal Palm  160x160 version
// or Handera   240x240 version
// or Hires 320x320 version

//#define NORMPALM                1
// #define HANDERA                 1    
//#define HIRES                   1

// makefile: CFLAGS = -O1 -palmos4
//#define SONY                    1

// signs after scroll point
#define AFTERSP     3000
// signs before scroll point
#define BEFORESP    1000     
// maximum len of field before inserting new record
#define MAXFLEN     (4096*4)

#ifdef NORMPALM //--------------------------------------------------------------
#define DEFSCREENWIDTH           160
#define DEFSCREENHIGHT           160

// scrollbar
#define SCROLLBARLEFT             53
#define SCROLLBARWIDTH           107
#define SCROLLBARHIGHT             5
#define MINSBCARSIZE               5              

// conbar
#define ICONROW1                   0
#define ICONROW2                 (ICONROW1+6)
#define BARHIGHT                 (ICONROW1+16)
#define CONBARTOP                (DEFSCREENHIGHT-BARHIGHT)

// graffiti state indicator position
#define CUSTOM_GSI_LEFT          126
#define GSI_LEFT                 (DEFSCREENWIDTH-8)
#define GSI_TOP                  (CONBARTOP-11)

// gettext dialog
#define GTHIGHT                   38
#define GSIEXT                     0

// open dialog
#define ODTOPY                     5
#define ODMAXHIGHT                (DEFSCREENHIGHT-2*ODTOPY)
#define ODDBLTOP                  14
#define ODBARHIGHT                31
#define ODBARTOP                   0

// bookmark dialog
#define MAXBOOK                   18
#define BOOKLISTLEFT              43
#define BOOKLISTWIDTH             75

// preferences dialog offset
#define DIAOFF                     0
#endif

#ifdef HANDERA //---------------------------------------------------------------
#define DEFSCREENWIDTH           240
#define DEFSCREENHIGHT           240

// scrollbar
#define SCROLLBARLEFT             69
#define SCROLLBARWIDTH           171
#define SCROLLBARHIGHT             7
#define MINSBCARSIZE               5              

// conbar
#define ICONROW1                   0
#define ICONROW2                 (ICONROW1+8)
#define BARHIGHT                 (ICONROW1+19)
#define CONBARTOP                DEFSCREENHIGHT-BARHIGHT

// graffiti state indicator position
#define CUSTOM_GSI_LEFT          196
#define GSI_LEFT                 (DEFSCREENWIDTH-12)
#define GSI_TOP                  (CONBARTOP-12)

// gettext dialog
#define GTHIGHT                   38
#define GSIEXT                     4

// open dialog
#define ODTOPY                     5
#define ODMAXHIGHT               (DEFSCREENHIGHT-2*ODTOPY)
#define ODDBLTOP                  19
#define ODBARHIGHT                31
#define ODBARTOP                   0

// bookmark dialog
#define MAXBOOK                   18
#define BOOKLISTLEFT              56
#define BOOKLISTWIDTH             75

// preferences dialog offset
#define DIAOFF                     5
#endif

#ifdef SONY //------------------------------------------------------------------
#define DEFSCREENWIDTH           160
#define DEFSCREENHIGHT           160

// scrollbar
#define SCROLLBARLEFT             53
#define SCROLLBARWIDTH           107
#define SCROLLBARHIGHT             5
#define MINSBCARSIZE               5              

// conbar
#define ICONROW1                   0
#define ICONROW2                 (ICONROW1+6)
#define BARHIGHT                 (ICONROW1+16)
#define CONBARTOP                DEFSCREENHIGHT-BARHIGHT

// graffiti state indicator position
#define CUSTOM_GSI_LEFT          126
#define GSI_LEFT                 (DEFSCREENWIDTH-10)
#define GSI_TOP                  (CONBARTOP-10)

// gettext dialog
#define GTHIGHT                   38
#define GSIEXT                     0

// open dialog
#define ODTOPY                     5
#define ODMAXHIGHT               (DEFSCREENHIGHT-2*ODTOPY)
#define ODDBLTOP                  14
#define ODBARHIGHT                31
#define ODBARTOP                   0

// bookmark dialog
#define MAXBOOK                   18
#define BOOKLISTLEFT              43
#define BOOKLISTWIDTH             75

// preferences dialog offset
#define DIAOFF                     0
#endif

#ifdef HIRES //------------------------------------------------------------------
#define DEFSCREENWIDTH           160
#define DEFSCREENHIGHT           160

// scrollbar
#define SCROLLBARLEFT             53
#define SCROLLBARWIDTH           107
#define SCROLLBARHIGHT             5
#define MINSBCARSIZE               5              

// conbar
#define ICONROW1                   0
#define ICONROW2                 (ICONROW1+6)
#define BARHIGHT                 (ICONROW1+16)
#define CONBARTOP                DEFSCREENHIGHT-BARHIGHT

// graffiti state indicator position
#define CUSTOM_GSI_LEFT          126
#define GSI_LEFT                 (DEFSCREENWIDTH-10)
#define GSI_TOP                  (CONBARTOP-10)

// gettext dialog
#define GTHIGHT                   38
#define GSIEXT                     0

// open dialog
#define ODTOPY                     5
#define ODMAXHIGHT               (DEFSCREENHIGHT-2*ODTOPY)
#define ODDBLTOP                  14
#define ODBARHIGHT                31
#define ODBARTOP                   0

// bookmark dialog
#define MAXBOOK                   18
#define BOOKLISTLEFT              43
#define BOOKLISTWIDTH             75

// preferences dialog offset
#define DIAOFF                     0
#endif

//------------------------------------------------------------------------------

// left of gettext
#define GTLEFT                     2

#define formID_main             1000
#define fieldID_main            1001

// position of main bar controls and bitmaps
#define bytelistID_maincont     1000
// number of main control buttons
#define MCBUTNUM                  14
// number of main controls
#define MCCONNUM                  18
// number of single control buttons 
#define MCSCBUTNUM                 9

// first main control bar id
#define FIRSTMCBID              1010
// main bar controls single bitmap
#define buttonID_new            1010
#define buttonID_open           1011
#define buttonID_search         1012
#define buttonID_replace        1013
#define buttonID_bookmark       1014
#define repeatbuttonID_upline   1015
#define repeatbuttonID_uppage   1016
#define repeatbuttonID_downline 1017
#define repeatbuttonID_downpage 1018
// main bar controls dual bitmap
#define buttonID_scrolldir      1019
#define buttonID_left           1020
#define buttonID_rigth          1021
#define buttonID_edit           1022
#define buttonID_filex          1023
#define TWIN_ID                 1024
#define SCROLLBAR_ID            1025
#define GRSTIN_ID               1026
#define READONLY_ID             1027

// main bar bitmaps single
#define bitmapID_new            1050
#define bitmapID_open           1051
#define bitmapID_search         1052
#define bitmapID_replace        1053
#define bitmapID_bookmark       1054
#define bitmapID_upline         1055
#define bitmapID_uppage         1056
#define bitmapID_downline       1057
#define bitmapID_downpage       1058
// main bar bitmaps dual
#define bitmapID_scrollver      1060
#define bitmapID_scrollhor      1061
#define bitmapID_edfalse        1062
#define bitmapID_edtrue         1063
#define bitmapID_file1          1064
#define bitmapID_file2          1065
#define bitmapID_leftgrey       1066
#define bitmapID_left           1067
#define bitmapID_rightgrey      1068
#define bitmapID_right          1069
// main bar bitmaps without controls
#define bitmapID_twinoff        1070
#define bitmapID_twinon         1071
#define bitmapID_sbar           1072
#define bitmapID_grstin0        1073
#define bitmapID_grstin1        1074
#define bitmapID_grstin2        1075
#define bitmapID_grstin3        1076
#define bitmapID_grstin4        1077
#define bitmapID_grstin5        1078
#define bitmapID_readonly       1079

#define menuID_main             1000
// file menu
#define menuitemID_new          1000
#define menuitemID_open         1001
#define menuitemID_close        1002
#define menuitemID_save         1003
#define menuitemID_saveas       1004
#define menuitemID_rename       1005
#define menuitemID_delete       1006
#define menuitemID_switch       1007
#define menuitemID_beam         1008
// edit menu
#define menuitemID_undo         1020
#define menuitemID_cut          1021
#define menuitemID_copy         1022
#define menuitemID_paste        1023
#define menuitemID_search       1024
#define menuitemID_replace      1025
#define menuitemID_editable     1026
#define menuitemID_jumpline     1027
// options menu
#define menuitemID_qedpref      1040
#define menuitemID_docpref      1041
#define menuitemID_bookmark     1042
#define menuitemID_bookchars    1043
#define menuitemID_scanbook     1044
#define menuitemID_togglebar    1045
#define menuitemID_twinview     1046
#define menuitemID_rotate       1047
#define menuitemID_resize       1048
#define menuitemID_dbinfo       1049
#define menuitemID_register     1050

#define alertID_info            1000
#define alertID_okcancel		1001

#define formID_prefs            1100
#define fieldID_filename        1101
#define labelID_fieldwidth      1102
#define popuptriggerID_fieldwidth 1103
#define listID_fieldwidth       1104
#define labelID_font            1105
#define checkID_editable        1106
#define checkID_ruled           1107
#define checkID_autoscan        1108
#define checkID_doubletap       1109
#define checkID_backup          1110
#define checkID_private         1111
#define checkID_fscroll         1112
#define checkID_hardbuttons     1113
#define checkID_autoindent      1114
#define checkID_monochrome      1115
#define checkID_readonly        1116
#define buttonID_prefscancel    1117
#define buttonID_prefsok        1118
#define bitmapID_can            1119
#define bitmapID_ok             1120
#define labelID_tab             1121
#define popuptriggerID_tab      1122
#define listID_tab              1123

#define FONTBUTTONIDSTART       1150
#define pushbuttonID_stdfont    1150
#define pushbuttonID_boldfont   1151
#define pushbuttonID_largefont  1152
#define pushbuttonID_repfont2   1155
#define pushbuttonID_repfont1   1156
#define pushbuttonID_lboldfont  1157
#define groupID_fontgroup          2

#define formID_busy             1200
#define labelID_busy            1201

// compress progress bar, message bar
#define CPPOSX					   0
#define CPHIGHT					(BARHIGHT-2)

#define formID_open             1300
#define listID_dblist           1301
#define buttonID_opencancel     1302
#define buttonID_openok         1303
#define popuptriggerID_catfilt  1304
#define popuptriggerID_catmake  1305
#define listID_catfilt          1306
#define bitmapID_catplus        1307
#define bitmapID_catminus       1308
#define bitmapID_catren         1309
#define bitmapID_catmake        1310
#define buttonID_catplus        1311
#define buttonID_catminus       1312
#define buttonID_catren         1313
#define pushbuttonID_actload    1314
#define pushbuttonID_actren     1315
#define pushbuttonID_actdel     1316
#define pushbuttonID_actcat     1317
#define listID_doccat           1318
#define bitmapID_actload        1319
#define bitmapID_actren         1320
#define bitmapID_actdel         1321
#define bitmapID_actcat         1322
#define bitmapID_opcapcat       1323
#define bitmapID_opcapfile      1324
#define groupID_actgroup           1

#define formID_bookmark         1350
#define listID_bookmark         1351
#define listID_booksort         1352
#define buttonID_bookadd        1353
#define pushbuttonID_bookjump   1354
#define pushbuttonID_bookren    1355
#define pushbuttonID_bookdel    1356
#define pushbuttonID_booksort   1357
#define bitmapID_bookadd        1358
#define bitmapID_bookjump       1359
#define bitmapID_bookren        1360
#define bitmapID_bookdel        1361
#define bitmapID_booksort       1362
#define groupID_bookgroup          2

#define menuID_edfunc           1399

#define formID_gettext          1400
#define fieldID_gettext         1401
#define fieldID_title           1402
#define buttonID_gtcancel       1403
#define buttonID_gtok           1404
#define bitmapID_gtcan          1405
#define bitmapID_gtok           1406
#define checkID_get1			1407

#define formID_searep           1500
#define labelID_srsea           1501
#define fieldID_seatext         1502
#define labelID_srrep           1503
#define fieldID_reptext         1504
#define pushbuttonID_srcase     1505
#define pushbuttonID_srtop      1506
#define pushbuttonID_srword     1507
#define bitmapID_srcase         1508
#define bitmapID_srtop          1509
#define bitmapID_srword         1510
#define buttonID_srnext         1511
#define buttonID_srreplace      1512
#define buttonID_srall          1513
#define buttonID_srcancel       1514    
#define bitmapID_srnext         1515
#define bitmapID_srreplace      1516
#define bitmapID_srall          1517
#define bitmapID_srcancel       1518

// search and replace flags hold by srbits
#define SRF_NONE                0x00
#define SRF_CASE                0x01
#define SRF_TOP                 0x02
#define SRF_UP                  0x04
#define SRF_WORD                0x08
#define SRF_REPFOUND            0x10
#define SRF_REPLACE             0x20
#define SRF_REPALL              0x40
#define SRF_GLOBSEA             0x80

#define formID_register         1600
#define bitmapID_qedlogo        1601
#define labelID_regunreg        1602
#define labelID_regreg          1603
#define fieldID_regcode         1604
#define labelID_regowner        1605
#define buttonID_register       1606
#define buttonID_reglater       1607
#define buttonID_regok          1608

// flag for function QfSetGetAttr
#define GETIPVIS                0x101

// definition of preferences flags1
#define FSCROLL                 0x01
#define HARDBUTTONS             0x02
#define AUTOINDENT              0x04
#define AUTOSCAN                0x08
#define DOUBLETAP               0x10
#define MONOCHROME              0x20

// definition of preferences flags2
// for handera only two bits
#define ROTATIONBITS            0x03
// for sony only two bits
#define DISPLAYEXPAND           0x0c

// definitions for uni1 flag
#define NOACTION                0x00
#define SCROLLBARSLIDE          0x01
#define SCREENSLIDE             0x02
#define SL_PHASE                0x04
#define SL_DIRUP                0x08
#define SL_DIRDOWN              0x10

// definitions for qflags, HSCROLL must be first bit
#define HSCROLL                 0x0001
#define FLOAD                   0x0002
#define REGFLAG                 0x0004
#define DBCHANGED               0x0008
#define READONLYCHANGE          0x0010
#define COLORDEVICE             0x0100
#define HIGHDENSITY             0x0200
#define COLORMODE               0x0400
#define TREOKEYBOARD            0x0800

// bits for RefreshDualControlBitmaps
#define REFRESHALL              0xff
#define EDITABLE                0x01
#define FILEANDTWIN             0x02
#define SCROLLDIR               0x04
#define LEFTRIGHT               0x08
#define READONLY                0x10
#define BACKERASE               0x80

// flags of routine CloseDB
#define SAVE_FIELD              0x01
#define SAVE_SPIP               0x02
#define AUTOBUSYOFF             0x04

// definition for sonybits 
#define DISPLAYCHANGED          0x01

// standard menu resources defined in <UIResources.h>
// definitions for rcp file
#define sysEditMenuUndoCmd          10000
#define sysEditMenuCutCmd           10001
#define sysEditMenuCopyCmd          10002
#define sysEditMenuPasteCmd         10003
#define sysEditMenuSelectAllCmd     10004
#define sysEditMenuKeyboardCmd      10006
#define sysEditMenuGraffitiCmd      10007
