/* qed.c V2.91 QRT 20060913 */

// line '#define DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS'
// in file 'BuildDefaults.h' must be commented out

#define ALLOW_ACCESS_TO_INTERNALS_OF_FORMS
#define ALLOW_ACCESS_TO_INTERNALS_OF_CONTROLS
#define ALLOW_ACCESS_TO_INTERNALS_OF_WINDOWS
#define ALLOW_ACCESS_TO_INTERNALS_OF_FIELDS
#define ALLOW_ACCESS_TO_INTERNALS_OF_BITMAPS

#include <PalmOS.h>
#include <DLServer.h>
#include <UIResources.h>

//#include <Treo\HsExt.h>
//#include <Treo\palmOne_68k.h>
#include <Treo\common\system\HsChars.h>
#include <Treo\68k\System\HsExt.h>
#include <Treo\68k\System\HsExtKbdUtils.h>


#include "qed.h"
#ifdef HANDERA
#include "Handera\vga.h"
#endif
#ifdef SONY
#include "Sony\SonyCLIE.h"
#endif

// if xinfo is defined a extended info is shown
// #define xinfo
#define docAppID         'REAd'         
#define docDBType        'TEXt'  
#define qedAppID         'QEDi'
#define QEDDBNAME        "QEDdata"
#define qedDBType        'Data'
#define PREFVERSION      270
#define DBVERSION        'Q270'

// static Boolean StartApplication(void);
static Boolean OpenDatabase(void);
static void EventLoop(void);
static Err UIAppSwitchTrap(UInt16, LocalID, UInt16, MemPtr);
static void StopApplication(void);
static Boolean FormEventMain(EventPtr);
static Boolean FormEventOpen(EventPtr);
static Boolean FormEventGetText(EventPtr);
static Boolean FormEventBookmark(EventPtr);
static Boolean FormEventSeaRep(EventPtr);
static Boolean FormEventPrefs(EventPtr);
static Boolean FormEventRegister(EventPtr);
static void DisplayChangeNotefication(SysNotifyParamType*);

DmOpenRef docDB, qedDB;
LocalID actdbID;
DmSearchStateType stateInfo;

/*
char ds1[20], ds2[20];
StrIToA(ds1, MyNumber);
StrIToA(ds2, MyNumber);
FrmCustomAlert(alertID_info, ds1, " ", ds2);
ErrFatalDisplayIf(true, "->aha");
ErrFatalDisplayIf(true, ds1);
char ds1[20];
StrIToA(ds1, mynumber);
FrmCustomAlert(alertID_info, ds1, " ", " ");
FrmCustomAlert(alertID_info, "aha", " ", " ");
*/

// maximum fieldsize
#define MAXFW       1280
// startrecord
#define STARTREC    1       
// standard record size
#define SRECSIZE    4096    

static void CheckField(void);
static void FldWriteScroll(void);
static void FldAutoScroll(void);
static void FldScrollH(Int16, Boolean);
static UInt16 RecSize(UInt16);
static void FldScrollV(UInt16, WinDirectionType);
static void UpdateField(Int32, Int32, Boolean);
static void SetCursor(Int32);
static void InsertRecord(void);
static Boolean Field2Recs(Boolean);
static void Mem2Rec(UInt16, MemPtr, UInt16);
static void Recs2Field(void);
static void FreeTextHandle(void);
static void Rec2Mem(MemPtr, UInt16, UInt16, UInt16);
static void DeCompress(UInt8*, MemPtr, UInt16);
static Boolean CheckSelection(void);
static void FetchPrefs(void);
static void OpenFirstDB(void);
static Err OpenDBid(LocalID, UInt16, Int32);
static MemPtr HandleDocAppInfo(LocalID);
static void GetNewDBName(Char*);
static void OpenNewEmptyDB(void);
static void CloseDB(UInt8);
static void SaveSpIp(void);
static void WriteDocInfo(DmOpenRef, LocalID, UInt32, UInt16, UInt8);
static void GetDBnames(void);
static void FreeOldNameMem(void);
static Boolean FetchDBname(Boolean, Char*, UInt16);
static Int16 comparF(MemPtr, MemPtr, Int32);
static void PrepOpenDialog(Boolean);
static void GetRecLens(void);
static void UpdateScrollBar(Boolean);
static void JumpScrollBarPosition(Int16);
static void DrawScrollBar(UInt8, UInt8);
static void RenameFileOD(void);
static void RenameFileME(void);
static void RenameFile(Char*, Char*);
static void DeleteFileOD(void);
static void DeleteFileME(void);
static void AskNewFile(void);
static void AskBookChars(void);
static UInt8 GetDocInfo(void);
static UInt16 GetUncompSize(MemPtr, UInt16);
static void ShowDBinfo(void);
static void ShowBusy(Char*);
static void Compress(MemPtr*, UInt16*);
static void SetFilePrefs(void);
static void Search(void);
static void Replace(void);
static void ReplaceAll(void);
static Int32 SearchText(Char*, Int32, Char*, UInt8);
static MemPtr InStrCl(Char*, UInt8, MemPtr, UInt16, UInt16, UInt8);
static UInt16 CRC_Calc(Char*);
static Char* GetUserName(Char*);
static void AskAddBookmark(void);
static void AddBookmark(Char*, Int32);
static void DeleteBookmark(UInt16);
static void GotoBookmark(UInt16);
static void AskRenameBookmark(void);
static void PrepareBookList(Boolean);
static void AutoScanBookmarks(void);
static void ScanBookmarks(Char*);
static void ChangeBookmark(UInt16, Int32, Char*);
static void SortBookmarks(Int16, Int16);
static void FreeOldBookMem(void);
static Int32 GetSpIpAbs(Int32*, Int32*);
static void SetCategoryFilter(UInt16);
static void PopupDocCatList(Char*, Int16);
static MemPtr GetObjectPointer(UInt16);
static void SetObjectPosition(UInt16, Int16, Int16);
static void ShowObject(UInt16);
static void HideObject(UInt16);
static void RefreshOpenDialog(void);
static void AddCategory(void);
static void DeleteCategory(void);
static void RenameCategory(void);
static void PopupGetText(Char*, Char*, Int16, Int16, Int16, MemPtr, Char*);
static void GetTextScroll(FieldPtr);
static void ExitGetText(FieldPtr, Boolean);
static void ReplaceFont(Char*, UInt16, UInt8);
static MemPtr SetCustomFont(UInt16, MemPtr);
static void PrepPrefsForm(void);
static void SavePrefs(FormPtr);
static void CtlSetV(UInt16, Boolean);
static Boolean CtlGetV(UInt16);
static void ChangePrefsDBN(void);
static void ChangePrefsPar(UInt16, Char*, Char*);
static void FetchDocAppInfo(LocalID, MemPtr);
static void UpdateDocAppInfo(LocalID, MemPtr);
static void HandleDoubleTap(void);
static Boolean CharValid(UInt8);
#ifdef NORMPALM
static void SimTapCount(EventPtr);             
#endif
static void SwitchBar(Boolean);
static void DrawBar(void);
static UInt8 DocAttribute(LocalID, UInt8*);
static void GlobalFindFind(FindParamsPtr);
static void GlobalFindGoTo(GoToParamsPtr);
static void SaveAs(void);
static void InitCompProgress(void);
static void ExitSeaRepForm(void);
static void QfSetTopLeft(Int16);
static Int16 QfGetTopLeft(void);
static Char* LstGetCurrentSelText(ListPtr);
static Boolean QfSetGetAttr(Int16);
static void JumpLine(void);
static eventsEnum ListKeystroke(Char, ListPtr);
static void SwitchFile(void);
static Boolean DbAttrStatus_CP_RO(void);
static Boolean DbAttrStatus_CP(void);
static Boolean DbAttrStatus_Par(Boolean);
static void Autoindent(void);
static WChar TabSpace(WChar);
static void HandleScreenSlide(Int16, Int16);
static void SizeForm(FormPtr);
static void RefreshMultiControlBitmaps(UInt8);
static void DrawBarBitmap(UInt8, UInt8, Boolean);
static void RefreshGSI(void);
static void BeamDoc(LocalID);
static Err WriteProc(const void*, UInt32*, void*);

// --------------------------------------------------------------------------

typedef struct{         // display properties:
    UInt8 fiwikey;      // fieldwidth key
    UInt8 unused;       // not used yet
    UInt16 font;        // font 
    Boolean ruled;      // ruled or not
    Boolean editable;   // editable or not
}dproperties;

// --------------------------------------------------------------------------

typedef struct{
    Char lastdbname[2][dmDBNameLength]; // name of last used database 0 and 1
    UInt8 actdbnanu;        // actual db name number (0 or 1)
    Boolean twinview;       // twin view of one database
    UInt16 regnumber;       // registration number or zero if not registered
    UInt8 flags1;           // definitions in qed.h
    UInt8 flags2;           // definitions in qed.h
    UInt8 tabwidth;         // tabwidth
    Boolean baron;          // status bar is on or off
    Char bookchars[13];     // characters for bookmarks 12 + zero
    UInt8 category;         // last selected category 
    dproperties dprop;      // display properies
}QedPrefsType;

static QedPrefsType QedPrefs;

static QedPrefsType QedDefaultPrefs =
{
    "",                 // name of last used database 0                          
    "",                 // name of last used database 1                        
    0,                  // actual db name number (0 or 1)
    false,              // twinview of one database      
    0,                  // registration number or zero if not registered       
    0,                  // flags1 definitions in qed.h               
    0,                  // flags2 definitions in qed.h
    0,                  // tabwidth = default TAB
    true,               // status bar on
    "°",                // characters for bookmarks 12 + zero                   
    255,                // last selected category = all                          
    0,                  // standard fieldwidth key 0=standard
    0,                  // unused
    stdFont,            // standard font                                       
    true,               // standard ruled or not                               
    true                // standard editable or not                            
};

// --------------------------------------------------------------------------

typedef struct{         
    Int32 dbversion;    // version of QED docappinfo
    Int32 spabs[2];     // absolute scroll position of DOC
    Int32 ipabs[2];     // absolute insertation point position of DOC
    dproperties dprop;  // display properies
}DocAppInfoType;        

typedef DocAppInfoType *DocAppInfoPtr;

static DocAppInfoType DocDefaultAppInfo, DocAppInfo;

// --------------------------------------------------------------------------

#define ALL                 255              
#define UNFILED             0                

typedef struct{
    UInt16  renamedCategories;
    Char    categoryLabels[dmRecNumCategories][dmCategoryLength];
    UInt8   categoryUniqIDs[dmRecNumCategories];
    UInt8   lastUniqID;     // Uniq IDs generated by the device are between
                            // 0 - 127.  Those from the PC are 128 - 255.
}QedCatAppInfoType;

typedef QedCatAppInfoType *QedCatAppInfoPtr;

static QedCatAppInfoType DefaultQedCategories =
{
    0xfffc,     // Deleted categories 1 = deleted

    "Unfiled",
    "Private",
    "",
    "",
    "", "", "", "",
    "", "", "", "",
    "", "", "", "",

    // Category Uniq IDs
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,

    // Last Uniq ID
    15
};

// --------------------------------------------------------------------------

typedef struct{         // gettext information
    Char title[20];     // title of form
    Char checktext1[10];// label of checkmark 1
    MemHandle texthand; // return handle of textfield
    Boolean check1;     // return value of checkmark 1
    Int16 x, y;         // click position of
    Int16 cwidth;       // character width of form
    UInt16 id;          // call form id (to return to)
    MemPtr funcptr;     // function pointer to jump to
}gtinfotype;    

gtinfotype *gtinfo;

// --------------------------------------------------------------------------

typedef struct{
    UInt16 menuid;              // id of menu who has called prefsdialog
    Char name[dmDBNameLength];  // dbname in prefsdialog
}prefstemptype;

prefstemptype *ptemp;

// --------------------------------------------------------------------------

#define NUMREPFONTS         2
#define REPFONT1            ledFont         
#define REPFONT2            symbol7Font         

#ifdef SONY             // number of font resources in font database
    #define NUMRESINDB     2
    #define QEDFONTDBNAME1      "QedFont1h"
    #define QEDFONTDBNAME2      "QedFont2h"
#else
    #define NUMRESINDB     1
    #define QEDFONTDBNAME1      "QedFont1"
    #define QEDFONTDBNAME2      "QedFont2"
#endif

struct{                 // replaced font structure
    UInt8 id;           // ID of replaced font (0-15)
    MemPtr sysptr;      // pointer to font in system font table
}repfont[NUMREPFONTS*NUMRESINDB];

// --------------------------------------------------------------------------

#ifdef HANDERA
const struct{
    UInt8 x; 
    UInt8 y;
    UInt8 bitmapid;
}BarItem[MCCONNUM] = {
     0,              ICONROW1,     bitmapID_new-FIRSTMCBID,
    14,              ICONROW1,     bitmapID_open-FIRSTMCBID,                            
    30,              ICONROW1,     bitmapID_search-FIRSTMCBID,                            
    43,              ICONROW1,     bitmapID_replace-FIRSTMCBID,
    BOOKLISTLEFT,    ICONROW1,     bitmapID_bookmark-FIRSTMCBID,                 
    69,              ICONROW2,     bitmapID_upline-FIRSTMCBID,
    86,              ICONROW2,     bitmapID_uppage-FIRSTMCBID,
   223,              ICONROW2,     bitmapID_downline-FIRSTMCBID,    
   206,              ICONROW2,     bitmapID_downpage-FIRSTMCBID, 
   103,              ICONROW2,     bitmapID_scrollver-FIRSTMCBID,
   121,              ICONROW2,     bitmapID_leftgrey-FIRSTMCBID,
   134,              ICONROW2,     bitmapID_rightgrey-FIRSTMCBID,
   146,              ICONROW2,     bitmapID_edfalse-FIRSTMCBID,
   160,              ICONROW2,     bitmapID_file1-FIRSTMCBID,
   169,              ICONROW2,     bitmapID_twinoff-FIRSTMCBID,
   SCROLLBARLEFT,    ICONROW1,     bitmapID_sbar-FIRSTMCBID,
   CUSTOM_GSI_LEFT,  ICONROW2,     bitmapID_grstin0-FIRSTMCBID,
   CUSTOM_GSI_LEFT,  ICONROW2,     bitmapID_readonly-FIRSTMCBID

};                               
#else
const struct{
    UInt8 x; 
    UInt8 y;
    UInt8 bitmapid;
}BarItem[MCCONNUM] = {
     0,              ICONROW1,     bitmapID_new-FIRSTMCBID,
    10,              ICONROW1,     bitmapID_open-FIRSTMCBID,                            
    23,              ICONROW1,     bitmapID_search-FIRSTMCBID,                            
    33,              ICONROW1,     bitmapID_replace-FIRSTMCBID,
    BOOKLISTLEFT,    ICONROW1,     bitmapID_bookmark-FIRSTMCBID,                 
    53,              ICONROW2,     bitmapID_upline-FIRSTMCBID,
    66,              ICONROW2,     bitmapID_uppage-FIRSTMCBID,
   147,              ICONROW2,     bitmapID_downline-FIRSTMCBID,    
   134,              ICONROW2,     bitmapID_downpage-FIRSTMCBID, 
    79,              ICONROW2,     bitmapID_scrollver-FIRSTMCBID,
    90,              ICONROW2,     bitmapID_leftgrey-FIRSTMCBID,
    98,              ICONROW2,     bitmapID_rightgrey-FIRSTMCBID,
   106,              ICONROW2,     bitmapID_edfalse-FIRSTMCBID,
   116,              ICONROW2,     bitmapID_file1-FIRSTMCBID,
   123,              ICONROW2,     bitmapID_twinoff-FIRSTMCBID,
   SCROLLBARLEFT,    ICONROW1,     bitmapID_sbar-FIRSTMCBID,
   CUSTOM_GSI_LEFT,  ICONROW2,     bitmapID_grstin0-FIRSTMCBID,
   CUSTOM_GSI_LEFT,  ICONROW2,     bitmapID_readonly-FIRSTMCBID
};                               
#endif
                                 
// --------------------------------------------------------------------------
                                                                  
FieldPtr fldptr;        // main field pointer
UInt16 lastrec;         // last record in document (without bookmark records) 
UInt32 dbsize;          // size of database
UInt32 acdbsize;        // actual size of database
Char actdbname[dmDBNameLength]; // actual name of database
Boolean editable;       // field is edatable or not
UInt16 *rlen;           // array of len of records in the database
Int32 frabs;            // absolute position of first record in field
UInt16 frec;            // first record of field
UInt16 lrec;            // last record of field
UInt16 numdb;           // number of databases
Char *naptr;            // packed array of database names
Char **npaptr;          // pointer to array of pointers to database names
UInt8 sbposold;         // old position of scrollbar
UInt8 sbsizeold;        // old size of scrollbar car
UInt16 sbscrs;          // scroll bar screen size
UInt16 lastfsize;       // last saved size of field
UInt8 compbyte;         // 01 = uncompressed 02 = compressed
UInt16 scrspeed;        // how much lines to scroll
UInt16 dbAttr;          // DB's attributes
UInt16 dbAttrCopy;      // copy of DB's attributes
UInt32 modnum;          // number of modifications of document
Char *bnptr=0;          // pointer to packed array of boomarknames
Char **bpaptr=0;        // pointer to array of pointers to bookmarknames
ListPtr bmlstptr;       // bookmark list pointer
UInt16 numbook;         // number of bookmarks
UInt8 nagcount=0;       // nagcounter
ListPtr dblstptr;       // database list pointer
ListPtr catlstptr;      // category list pointer
Boolean hideprivate=false;  // hide or not 
UInt32 osversion;       // OS version
UInt32 cptrig=0;        // compress progress number of uncompressed bytes, also used when launching
UInt32 cprest=0;        // compress progress integer step operation rest, also used when launching
UInt16 cpx;             // x-position of progress
Int16 lbooksel;         // last selected bookmark
MemHandle seathand;     // search text handle
MemHandle repthand;     // replace text handle
UInt8 srbits=SRF_NONE;  // bits for searching and replacing
LocalID launchdbid=0;   // dbid to launch
Int16 ppsch;            // palm pilot screen height
Int16 ppscw;            // palm pilot screen width
Int16 fieldwidth;       // fieldwidth in pixel
FontID stdfh;           // standard font height
IndexedColorType barbackcol;    // color of bar background
Int16 lasttlx;          // last topleft x used for QfSetTopLeft
UInt8 laststate;        // last state of GSI
UInt16 qflags;          // look in .h file for definitions
void* uiappswitchptr;   // trap UIAppSwitch Function

// variables for universal use, be careful
UInt8 uni1;             // scrollbar active flag
UInt8 uni2;             // scrollbar movement offset, number of taps for FormEventOpen
Int16 x, y;             // pen coordinates in FormEventMain and FormEventBookmark

// second use of variables
#define SLEN    cptrig
#define APOS    cprest

#ifdef HANDERA
Boolean lee;
#endif

#ifdef SONY
UInt16 HRrefNum;
UInt16 SilkRefNum;
UInt8 sonybits;
#endif

//*****************************************************************
//
// PilotMain
//
//*****************************************************************

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
Boolean error;
    
    switch(cmd){
        case sysAppLaunchCmdNormalLaunch:                   // normal launch
            break;

        case sysAppLaunchCmdOpenDB:                         // a launcher starts qed with a doc
            launchdbid = ((SysAppLaunchCmdOpenDBType*)cmdPBP)->dbID;
            break;    
        
        case sysAppLaunchCmdFind:                           // global find was activated
            if(launchFlags & sysAppLaunchFlagSubCall)       // test if app is already launched
                GlobalFindFind((FindParamsPtr)cmdPBP);      // yes it is
            
            return 0;
            
        case sysAppLaunchCmdGoTo:            
            if(launchFlags & sysAppLaunchFlagSubCall){      // test if app is already launched
                GlobalFindGoTo((GoToParamsPtr)cmdPBP);      // yes, global find goto   
                return 0;
            }
            else{                                           // external launch
                launchdbid = ((GoToParamsPtr)cmdPBP)->dbID;
                SLEN = -1;                                  // flag use APOS for new cursor position

                if(((GoToParamsPtr)cmdPBP)->searchStrLen == -1){    // flag matchCustom to be valid  
                    APOS = ((GoToParamsPtr)cmdPBP)->matchCustom;
                }
                else{
                    if(((GoToParamsPtr)cmdPBP)->recordNum == -1)
                        SLEN = 0;                           // flag restore old cursor position
                    else
                        APOS = ((GoToParamsPtr)cmdPBP)->matchPos;   // only positions < 64 K
                }
                
                break;
            }
                            
        default:                                            // startcode unknown or not supported
            return 0;
    }

    if(error = OpenDatabase())
        return error;
    
    FrmGotoForm(formID_main);
    EventLoop();
    StopApplication();
    return 0;
}

//*****************************************************************
//
// StartApplication
//
//*****************************************************************

//static Boolean StartApplication(void)
//{
//Boolean error;

//    error = OpenDatabase();
//    if(error) return error;
//    FrmGotoForm(formID_main)

//*****************************************************************
//
// OpenDatabase
//
//*****************************************************************

static Boolean OpenDatabase(void)
{
//MemHandle RecHandle;
//MemPtr RecPointer;
//char nullstring = 0;
//UInt16 index = 0;
LocalID dbid, appInfoID;
MemHandle ihand;
QedCatAppInfoPtr infoptr;
RGBColorType rgb;
UInt32 winVersion=0;
UInt32 keyboardType=0;
UInt32 depth=0;
#ifdef HIRES
UInt32 attr=0;
#endif
#ifdef SONY
SonySysFtrSysInfoP sonySysFtrSysInfoP;
UInt16 cardNo;
LocalID dbID;
UInt32 width=320, height=320;
#endif

    #ifdef HIRES
    FtrGet(sysFtrCreator, sysFtrNumWinVersion, &winVersion);

    if(winVersion >= 4){
        WinScreenGetAttribute(winScreenDensity, &attr);
    
        if(attr == kDensityDouble)
            qflags |= HIGHDENSITY;
    }    
    #endif

    #ifdef SONY    
    if(FtrGet(sonySysFtrCreator, sonySysFtrNumSysInfoP, (UInt32*)&sonySysFtrSysInfoP))
        goto NOSONY;
        
    if(!(sonySysFtrSysInfoP->libr & sonySysFtrSysInfoLibrHR))   // open HRLib
        goto NOSONY;                                            // error, HRLib not available
        
    if(SysLibFind(sonySysLibNameHR, &HRrefNum)){                // == sysErrLibNotFound
        if(SysLibLoad('libr', sonySysFileCHRLib, &HRrefNum)){   
            HRrefNum = 0;                                       // error, can't load HRLib
            goto NOSONY;                                        // (sysInvalidRefNum is in HRrefNum)
        }
    }
    
    HROpen(HRrefNum);
    HRWinScreenMode(HRrefNum, winScreenModeSet, &width, &height, NULL, NULL);

    // open SilkLib
    if(sonySysFtrSysInfoP->extn&sonySysFtrSysInfoExtnSilk && sonySysFtrSysInfoP->libr&sonySysFtrSysInfoLibrSilk){
        if(SysLibFind(sonySysLibNameSilk, &SilkRefNum))
            if(SysLibLoad('libr', sonySysFtrSysInfoLibrSilk, &SilkRefNum))   
                SilkRefNum = 0;
         
        if(SilkRefNum){        
            SilkLibOpen(SilkRefNum);
            SilkLibEnableResize(SilkRefNum);
            SysCurAppDatabase(&cardNo, &dbID);  // register for sysNotifyDisplayChangeEvent
            SysNotifyRegister(cardNo, dbID, sysNotifyDisplayChangeEvent, (SysNotifyProcPtr)DisplayChangeNotefication, sysNotifyNormalPriority, NULL);
        }
    }
    
    NOSONY:
    if(!HRrefNum){
        FrmCustomAlert(alertID_info, "No ", "Sony hires device", "!");
        return true;
    }
    #endif               

    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osversion);
    FtrGet(hsFtrCreator, hsFtrIDTypeOfKeyboard, &keyboardType);

    if(keyboardType&hsFtrValKeyboardQwerty)
       qflags |= TREOKEYBOARD;

    qedDB = DmOpenDatabaseByTypeCreator(qedDBType, qedAppID, dmModeReadWrite);
    
    if(!qedDB){
        if(DmCreateDatabase(0, QEDDBNAME, qedAppID, qedDBType, false)) return 1;
        qedDB = DmOpenDatabaseByTypeCreator(qedDBType, qedAppID, dmModeReadWrite);
        //RecHandle = DmNewRecord(qedDB, &index, 1);
        //RecPointer = MemHandleLock(RecHandle);
        //DmWrite(RecPointer, 0, &nullstring, 1);
        //MemPtrUnlock(RecPointer); 
        //DmReleaseRecord(qedDB, index, true);  
    }

    appInfoID = DmGetAppInfoID(qedDB);
    
    if(!appInfoID){
        dbid = DmFindDatabase(0, QEDDBNAME);
        ihand = DmNewHandle(qedDB, sizeof(DefaultQedCategories));
        infoptr = MemHandleLock(ihand);
        DmWrite(infoptr, 0, &DefaultQedCategories, sizeof(DefaultQedCategories));
        appInfoID = MemHandleToLocalID(ihand);
        DmSetDatabaseInfo(0, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appInfoID, NULL, NULL, NULL);
        MemHandleUnlock(ihand);    
    }
    
    FetchPrefs();

    #ifdef SONY
    if(SilkRefNum)
        SilkLibResizeDispWin(SilkRefNum, (QedPrefs.flags2&DISPLAYEXPAND)>>2);
    #endif
    
    // GetDBnames();

    seathand = DmNewHandle(qedDB, 23);
    DmStrCopy(MemHandleLock(seathand), 0, "");
    MemHandleUnlock(seathand);
    
    repthand = DmNewHandle(qedDB, 23);
    DmStrCopy(MemHandleLock(repthand), 0, "");
    MemHandleUnlock(repthand);
                        
    // replace fonts if available    
    if(osversion < 0x05000000){     // does not work for OS5 - yet
        uiappswitchptr = SysGetTrapAddress(sysTrapSysUIAppSwitch);
        SysSetTrapAddress(sysTrapSysUIAppSwitch, UIAppSwitchTrap);
        ReplaceFont(QEDFONTDBNAME1, REPFONT1, 0);
        ReplaceFont(QEDFONTDBNAME2, REPFONT2, 1);
    }
    
    if(osversion >= 0x03503000){
        WinScreenMode(winScreenModeGetDefaults, NULL, NULL, &depth, NULL);

        if(depth > 4){
            rgb.r = 0xa0; rgb.g = 0xa0; rgb.b = 0xff;
            barbackcol = WinRGBToIndex(&rgb);
            qflags |= COLORDEVICE | COLORMODE;

            if(QedPrefs.flags1&MONOCHROME){
                depth = 1;
                WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL);
                qflags &= ~COLORMODE;
            }
        }
    }
            
    return false;       // no error
}

//*****************************************************************
//
// EventLoop
//
//*****************************************************************

static void EventLoop(void)
{
UInt16 err, formID;
FormPtr form;
EventType event;
WChar c;
Boolean flag;

    do{
        EvtGetEvent(&event, evtWaitForever);
        flag = true;
            	
    	if(event.eType == keyDownEvent){
            c = event.data.keyDown.chr;            
            
            if(c>=hard1Chr && c<=hard4Chr && QedPrefs.flags1&HARDBUTTONS)
                flag = false;
        }

        if(flag && SysHandleEvent(&event)){
            //if(event.eType==penUpEvent && QedPrefs.baron && !(dbAttr&dmHdrAttrReadOnly))
            //    RefreshGSI();           // catch GSI event
            
            continue;
        }
        
        if(MenuHandleEvent((void *)0, &event, &err)) 
            continue;

        if(event.eType == frmLoadEvent){
            formID = event.data.frmLoad.formID;
            
            // if(!(form=FrmGetFormPtr(formID)))
            form = FrmInitForm(formID);
            FrmSetActiveForm(form);
            
            switch(formID){
                case formID_main:
                    fldptr = GetObjectPointer(fieldID_main);
                    FrmSetEventHandler(form, (FormEventHandlerPtr)FormEventMain);
                    break;
                case formID_gettext:
                    FrmSetEventHandler(form, (FormEventHandlerPtr)FormEventGetText);
                    break;
                case formID_bookmark:
                    bmlstptr = GetObjectPointer(listID_bookmark);
                    FrmSetEventHandler(form, (FormEventHandlerPtr)FormEventBookmark);
                    break;
                case formID_searep:
                    FrmSetEventHandler(form, (FormEventHandlerPtr)FormEventSeaRep);
                    break;
                case formID_open:
                    dblstptr = GetObjectPointer(listID_dblist);
                    catlstptr = GetObjectPointer(listID_catfilt);
                    FrmSetEventHandler(form, (FormEventHandlerPtr)FormEventOpen);
                    break;
                case formID_prefs:
                    FrmSetEventHandler(form, (FormEventHandlerPtr)FormEventPrefs);
                    break;
                case formID_register:
                    FrmSetEventHandler(form, (FormEventHandlerPtr)FormEventRegister);
                    break;
            }
        }
        
        FrmDispatchEvent(&event); 
    }while(event.eType != appStopEvent);   
}

//*****************************************************************
//
// UIAppSwitchTrap
//
//*****************************************************************

static Err UIAppSwitchTrap(UInt16 cardNo, LocalID dbID, UInt16 cmd, MemPtr cmdPBP)
{
UInt16 i;
MemPtr repptr;

    if(docDB)
        SaveSpIp();                                     

    for(i=0; i<NUMREPFONTS*NUMRESINDB; i++){
       if(repfont[i].sysptr){
            repptr = SetCustomFont(repfont[i].id, repfont[i].sysptr);
            MemPtrUnlock(repptr);
        }
    }    

    SysSetTrapAddress(sysTrapSysUIAppSwitch, uiappswitchptr);
    return SysUIAppSwitch(cardNo, dbID, cmd, cmdPBP);    
}

//*****************************************************************
//
// StopApplication
//
//*****************************************************************

static void StopApplication(void)
{
#ifdef SONY
UInt16 cardNo;
LocalID dbID;
#endif

    StrCopy(QedPrefs.lastdbname[QedPrefs.actdbnanu], qflags&FLOAD ? actdbname : "");
    PrefSetAppPreferences(qedAppID, 1, PREFVERSION, &QedPrefs, sizeof(QedPrefsType), true);
    CloseDB(osversion<0x05000000 ? SAVE_FIELD : SAVE_FIELD|SAVE_SPIP);
    FreeOldNameMem();
    FrmCloseAllForms();
    MemHandleFree(seathand);
    MemHandleFree(repthand);
    DmCloseDatabase(qedDB);

    #ifdef SONY
    if(SilkRefNum){
        SysCurAppDatabase(&cardNo, &dbID);
        SysNotifyUnregister(cardNo, dbID, sysNotifyDisplayChangeEvent, sysNotifyNormalPriority);
        
        if(osversion < 0x05000000)
            SilkLibResizeDispWin(SilkRefNum, silkResizeNormal);
        
        SilkLibDisableResize(SilkRefNum);
        SilkLibClose(SilkRefNum);
    }
    
    HRWinScreenMode(HRrefNum, winScreenModeSetToDefaults, NULL, NULL, NULL, NULL);
    HRClose(HRrefNum);
    #endif
}

//*****************************************************************
//
// FormEventMain
//
//*****************************************************************

static Boolean FormEventMain(EventPtr event)
{
Boolean handled=false;
FieldAttrType attr;
WChar c;
Boolean vf;
Int16 x, y;
UInt16 nd;
FormPtr form;
IndexedColorType prevcol;
#ifdef HANDERA
VgaScreenModeType mode;
VgaRotateModeType rotation;
#endif

    if(qflags&FLOAD){
        switch(event->eType){
            case keyDownEvent:          // a key was pressed
                
                FldGetAttributes(fldptr, &attr);                
                vf = attr.insPtVisible;
                c = event->data.keyDown.chr;                
                
                if(editable && vf && !(dbAttr&dmHdrAttrReadOnly)){
                    if(c<=chrGroupSeparator && (c==chrHorizontalTabulation || c==chrGroupSeparator || c==chrFileSeparator))
                        c = TabSpace(c);
                    
                    if(c){
                        if(qflags&TREOKEYBOARD && c==hsChrPostProcess){
                            HsPostProcessPopupList(false, 0);
                        }
                        else{
                            FldHandleEvent(fldptr, event);
                        }
                        
                        #ifdef SONY                     // because of an error in NR70V GSI
                        if(laststate == 3)              // shift key 'hangs' 
                            GrfSetState(false, false, false);
                        #endif
                        
                        if(c==chrLineFeed && QedPrefs.flags1&AUTOINDENT)
                            Autoindent();
                    } 
                                        
                    if(fieldwidth > ppscw)
                        FldWriteScroll();
                    
                    CheckField();
                }                
                
                if(c <= chrUnitSeparator){                          // 0x1f
                    switch(c){                                      // GT=GoType, PF=PalmFoldout keyboard
                        case chrVerticalTabulation:                 // 0x0b
                            FldScrollV(scrspeed, winUp);            //(GT alt, PF Fn) up button
                            break;
                        case chrFormFeed:                           // 0x0c
                            FldScrollV(scrspeed, winDown);          //(GT alt, PF Fn) down button
                            break;
                        case chrRecordSeparator:                    // 0x1e
                            if(!editable || !vf) FldScrollV(1, winUp);    // (GT, PF) up button
                            break;
                        case chrUnitSeparator:                      // 0x1f
                            if(!editable || !vf) FldScrollV(1, winDown);  // (GT, PF) down button
                            break; 
                        case chrFileSeparator:                      // 0x1c
                            if(!editable){
                                FldReleaseFocus(fldptr);            // sidescroll without focus
                                FldScrollH(ppscw-16, true);         // (GT, PF) left button
                            }
                            break;
                        case chrGroupSeparator:                     // 0x1d
                            if(!editable){
                                FldReleaseFocus(fldptr);            // sidescroll without focus
                                FldScrollH(-ppscw+16, true);        // (GT, PF) right button 
                            }
                            break;
                    }
                }

                if(c >= hard1Chr){
                    switch(c){                         
                        case hard2Chr:                              // 0x205
                            FldScrollV(1, winUp);                   // hard key 2 scroll one up
                            break;
                        case hard3Chr:                              // 0x206
                            FldScrollV(1, winDown);                 // hard key 3 scroll one down
                            break; 
                        case hard1Chr:                              // 0x204
                            FldReleaseFocus(fldptr);                // sidescroll without focus
                            FldScrollH(MAXFW, true);                // hard key 1 scroll left (home)
                            break;
                        case hard4Chr:                              // 0x207
                            FldReleaseFocus(fldptr);                // sidescroll without focus
                            FldScrollH(-ppscw+16, true);            // hard key 4 scroll right
                            break;
                    }
                }
                
                handled = true;
                break;
                        
            case penMoveEvent:
                if(uni1&SCREENSLIDE){
                    HandleScreenSlide(event->screenX, event->screenY);
                    handled = true;
                }
                else if(uni1&SCROLLBARSLIDE){
                    JumpScrollBarPosition(event->screenX);
                }
                
                break;

            case penDownEvent:
                #ifdef NORMPALM
                if(osversion < 0x03503000)
                    SimTapCount(event);             
                #endif
                x = event->screenX;
                y = event->screenY;
                nd = true;
                
                #ifdef SONY
                if(SilkRefNum && ppsch==DEFSCREENHIGHT+80 && y<10){
                    if(x > DEFSCREENWIDTH-10){  // upper right edge
                        SilkLibResizeDispWin(SilkRefNum, silkResizeToStatus);
                        handled == true;
                        break;
                    }
                    else if(x<10){              // upper left edge
                        event->eType = appStopEvent;
                        EvtAddEventToQueue(event);
                        handled == true;
                        break;
                    }
                }                
                #endif
                
                if(QedPrefs.baron){
                    if(y >= ppsch-BARHIGHT){
                        nd = false;
                        
                        if(y<=ppsch-BARHIGHT+ICONROW1+SCROLLBARHIGHT+1 && x>=SCROLLBARLEFT && x<=SCROLLBARLEFT+SCROLLBARWIDTH)
                            JumpScrollBarPosition(x);
                    }
                }
            
                if(nd && !editable){
                    HandleScreenSlide(x, y);
                    handled = true;         // prevents marking of text in field
                }
            
                break;

            case penUpEvent:
                if(uni1&SCREENSLIDE){
                    if(uni1&SL_PHASE) FldDrawField(fldptr);
                    CheckField();    
                }
                
                uni1 = NOACTION;
                break;

            case nilEvent:          // periodically check GSI for NR70V keyboard
                if(QedPrefs.baron && !(dbAttr&dmHdrAttrReadOnly))
                    RefreshGSI();
                
                break;

            case ctlRepeatEvent:    // a repeat button is pressed
                // prevent inverse control graphics after compressing progrsss bar 
                // CtlHandleEvent(GetObjectPointer(event->data.ctlRepeat.controlID), event);
                vf = FldDirty(fldptr);
                
                switch(event->data.ctlRepeat.controlID){
                    case repeatbuttonID_upline:
                        FldScrollV(1, winUp);
                        break;
                    case repeatbuttonID_uppage:
                        FldScrollV(scrspeed, winUp);
                        break;
                    case repeatbuttonID_downline:
                        FldScrollV(1, winDown);
                        break;
                    case repeatbuttonID_downpage:
                        FldScrollV(scrspeed, winDown);
                        break;
                }
                
                if(vf != FldDirty(fldptr))
                    DrawBarBitmap(event->data.ctlRepeat.controlID-FIRSTMCBID, 0, true);

                handled = false;
                break;

            case ctlSelectEvent:    // a control button was pressed and released
                switch(event->data.ctlSelect.controlID){
                    case buttonID_edit:
                        editable = !editable;
                        RefreshMultiControlBitmaps(EDITABLE|BACKERASE);
                        handled = true;
                        break;
                    case buttonID_scrolldir:
                        qflags ^= HSCROLL;
                        RefreshMultiControlBitmaps(SCROLLDIR|BACKERASE);
                        UpdateScrollBar(false);
                        handled = true;
                        break;
                    case buttonID_left:
                        FldScrollH(MAXFW, true);
                        handled = true;
                        break;
                    case buttonID_rigth:
                        FldScrollH(-ppscw+16, true);
                        handled = true;
                        break;
                    case buttonID_bookmark:
                        AutoScanBookmarks();
                        FrmPopupForm(formID_bookmark);
                        handled = true;
                        break;
                    case buttonID_search:
                        srbits &= ~SRF_REPLACE;     // delete replace flag
                        FrmPopupForm(formID_searep);
                        handled = true;
                        break;
                    case buttonID_replace:
                        if(DbAttrStatus_CP() && editable){
                            srbits |= SRF_REPLACE;  // set replace flag
                            FrmPopupForm(formID_searep);
                        }
                        handled = true;
                        break;
                }
                break;
            
            case fldEnterEvent:     // pen was set down
                if(editable && QedPrefs.flags1&DOUBLETAP && event->tapCount==2){
                    event->tapCount = 1;
                    FldHandleEvent(fldptr, event);
                    HandleDoubleTap();
                }
                else{
                    FldHandleEvent(fldptr, event);
                }

                handled = true;
                break;
            
            case fldChangedEvent: 
                if(!CheckSelection())   // selection scrolled field
                    CheckField();       // cursor scrolled field or text was pasted
                
                break;
                
            case menuEvent:
                switch(event->data.menu.itemID){ 
                    case menuitemID_editable:
                        MenuEraseStatus(NULL);  // prevents restoring changed graphics
                        editable = !editable;
                        RefreshMultiControlBitmaps(EDITABLE|BACKERASE);
                        handled = true;
                        break;
                    case menuitemID_twinview:
                        MenuEraseStatus(NULL);  // prevents restoring changed graphics
                        QedPrefs.twinview = !QedPrefs.twinview;
                        RefreshMultiControlBitmaps(FILEANDTWIN|BACKERASE);
                        handled = true;
                        break;
                    case menuitemID_save:
                        Field2Recs(true);
                        handled = true;
                        break;
                    case menuitemID_saveas:
                        Field2Recs(true);
                        PopupGetText("Save as:", actdbname, GTLEFT, ppsch-BARHIGHT-GSIEXT-8, 31, *SaveAs, "Compress");
                        handled = true;
                        break;
                    case menuitemID_cut:
                        if(DbAttrStatus_CP() && editable){
                            FldCut(fldptr);     // ignores field editable flag
                            CheckField();
                        }
                        handled = true;
                        break;
                    case menuitemID_copy:
                        FldCopy(fldptr);
                        handled = true;
                        break;
                    case menuitemID_paste:
                        if(DbAttrStatus_CP() && editable){
                            FldPaste(fldptr);
                            CheckField();
                        }
                        handled = true;
                        break;
                    case menuitemID_undo:
                        if(editable){
                            FldUndo(fldptr);
                            CheckField();
                        }
                        handled = true;
                        break;
                    case menuitemID_search:
                        srbits &= ~SRF_REPLACE;         // delete replace flag
                        FrmPopupForm(formID_searep);
                        handled = true;
                        break;
                    case menuitemID_replace:
                        if(DbAttrStatus_CP() && editable){
                            srbits |= SRF_REPLACE;      // set replace flag
                            FrmPopupForm(formID_searep);
                        }
                        handled = true;
                        break;
                    case menuitemID_jumpline:
                        PopupGetText("Jump Line:", "", GTLEFT, ppsch-BARHIGHT-GSIEXT+4, 12, *JumpLine, "");
                        handled = true;
                        break;
                    case menuitemID_close:
                        MenuEraseStatus(NULL);  // prevents restoring changed graphics
                        CloseDB(SAVE_FIELD|SAVE_SPIP|AUTOBUSYOFF);
                        DrawBar();
                        handled = true;
                        break;
                    case menuitemID_bookmark:
                        AutoScanBookmarks();
                        FrmPopupForm(formID_bookmark);
                        handled = true;
                        break;
                    case menuitemID_scanbook:
                        if(DbAttrStatus_CP())
                            ScanBookmarks(QedPrefs.bookchars);
                        handled = true;
                        break;
                    case menuitemID_togglebar:
                        MenuEraseStatus(NULL);  // prevents restoring changed graphics
                        QedPrefs.baron = !QedPrefs.baron;
                        SwitchBar(true);
                        handled = true;
                        break;
                    case menuitemID_qedpref:
                    case menuitemID_docpref:
                        ptemp = MemPtrNew(sizeof(prefstemptype));
                        ptemp->menuid = event->data.menu.itemID;
                        FrmPopupForm(formID_prefs);
                        handled = true;
                        break;
                    case menuitemID_bookchars:
                        PopupGetText("Book Chars:", QedPrefs.bookchars, GTLEFT, ppsch-BARHIGHT-GSIEXT+4, 12, *AskBookChars, "");
                        handled = true;
                        break;
                    case menuitemID_dbinfo:
                        ShowDBinfo();
                        handled = true;
                        break;                    
                    case menuitemID_rename:
                        if(DbAttrStatus_CP_RO())
                            PopupGetText("Rename File:", actdbname, GTLEFT, ppsch-BARHIGHT-GSIEXT+4, 31, *RenameFileME, "");
                        handled = true;
                        break;                    
                     case menuitemID_delete:
                        if(DbAttrStatus_CP_RO())
                            PopupGetText("Delete File:", actdbname, GTLEFT, ppsch-BARHIGHT-GSIEXT+4, 31, *DeleteFileME, "");
                        handled = true;
                        break;                    
                    case menuitemID_beam:
                        if(osversion >= 0x03003000){
                            Field2Recs(true);
                            BeamDoc(actdbID);
                        }
                        handled = true;
                        break;                    
                }
                break;
            
            case menuCmdBarOpenEvent:
                if(!editable || dbAttr&dmHdrAttrReadOnly)
                    event->data.menuCmdBarOpen.preventFieldButtons = true;

                break;
        }
    }
    
    if(!handled){
        form = FrmGetActiveForm();

        switch(event->eType){
            case ctlSelectEvent:    // a control button was pressed and released
                switch(event->data.ctlSelect.controlID){
                    case buttonID_filex:
                        SwitchFile();
                        handled = true;
                        break;
                    case buttonID_open:
                        FrmPopupForm(formID_open);
                        handled = true;
                        break;
                    case buttonID_new:
                        AskNewFile();
                        handled = true;
                        break;
                }
                break;
            case menuEvent:
                switch (event->data.menu.itemID){ 
                    case menuitemID_switch:
                        MenuEraseStatus(NULL);  // prevents restoring changed graphics
                        SwitchFile();
                        handled = true;
                        break;
                    case menuitemID_open:
                        FrmPopupForm(formID_open);
                        handled = true;
                        break;
                    case menuitemID_new:
                        AskNewFile();
                        handled = true;
                        break;
                    
                    #ifdef HANDERA
                    case menuitemID_rotate:
                        VgaGetScreenMode(&mode, &rotation);
                        rotation = VgaRotateSelect(rotation);
                        VgaSetScreenMode(screenMode1To1, rotation);
                        QedPrefs.flags2 = (QedPrefs.flags2 & ~ROTATIONBITS) | rotation;
                        handled = true;
                        break;
                    #endif
                    
                    #ifdef SONY
                    case menuitemID_resize:     // only visible if SilkRefNum
                        nd =(QedPrefs.flags2 & DISPLAYEXPAND) >> 2;
                        
                        if(nd == silkResizeMax){
                            nd = silkResizeToStatus;
                        }
                        else if(nd == silkResizeToStatus){
                            nd = silkResizeMax;
                        }
                        else if(nd == silkResizeNormal){
                            SilkLibResizeDispWin(SilkRefNum, silkResizeToStatus);
                            nd = silkResizeMax;
                        }
                        
                        SilkLibResizeDispWin(SilkRefNum, nd);
                        handled = true;
                        break;
                    #endif
                                        
                    case menuitemID_register:
                        FrmPopupForm(formID_register);
                        handled = true;
                        break;
                }            
                break;

            case frmUpdateEvent:
                if((nd=event->data.frmUpdate.updateCode) != frmRedrawUpdateCode){
                    CloseDB(SAVE_FIELD|SAVE_SPIP|AUTOBUSYOFF);
                    OpenDBid(DmFindDatabase(0, npaptr[nd]), 0, 0);
                }
                else{                    
                    FrmDrawForm(form);
                }                
                handled = true;
                break;
            
            #ifdef HANDERA
            case displayExtentChangedEvent:
                SizeForm(form);
                FrmDrawForm(form);
                SwitchBar(true);
                handled = true;
                break;
            
            case menuOpenEvent:
                MenuAddItem(menuitemID_twinview, menuitemID_rotate, 'G', "Rotate");
                break;
            #endif    
                
            #ifdef SONY
            case menuOpenEvent:
                if(SilkRefNum)
                    MenuAddItem(menuitemID_twinview, menuitemID_resize, NULL, "Resize Display");
                break;
            #endif            
            
            case winEnterEvent:
                #ifdef SONY
                if(sonybits & DISPLAYCHANGED){
                    sonybits &= ~DISPLAYCHANGED;                 
                    SizeForm(form);
                    FrmDrawForm(form);
                    SwitchBar(true);
                }
                #endif
                break;

            case frmOpenEvent:
                #ifdef HANDERA
                VgaSetScreenMode(screenMode1To1, QedPrefs.flags2&ROTATIONBITS);
                #endif
                SizeForm(form);
                FrmDrawForm(form);
                stdfh = FntLineHeight();
                OpenFirstDB();
                handled = true;
                break;                        
        }
    }
    
    return handled;
}

//*****************************************************************
//
// FormEventOpen
//
//*****************************************************************

static Boolean FormEventOpen(EventPtr event)
{
FormPtr form;
Boolean handled=false;
UInt16 index;
Int8 c;
UInt16 newdbno;
LocalID dbid;
UInt16 dbatt;
Char *seltext;
UInt16 obid;
RectangleType rect;
UInt16 i, numob;
FormObjectKind obtype;
Coord btop;

    form = FrmGetActiveForm();
    index = FrmGetControlGroupSelection(form, groupID_actgroup);
    
    switch(event->eType){        
        case keyDownEvent:          // a key was pressed
            ListKeystroke(event->data.keyDown.chr, dblstptr);
            handled = true;
            break;
        
        case ctlSelectEvent:        // a control button was pressed and released
            seltext = LstGetCurrentSelText(catlstptr);
            x += 4; y += 2;

            switch(event->data.ctlSelect.controlID){                
                case buttonID_catplus:
                    PopupGetText("Add Category:", "", x, y, 15, *AddCategory, "");
                    handled = true;
                    break;
                case buttonID_catminus:
                    PopupGetText("Del Category:", seltext, x, y, 15, *DeleteCategory, "");
                    handled = true;
                    break;
                case buttonID_catren:
                    PopupGetText("Ren Category:", seltext, x, y, 15, *RenameCategory, "");
                    handled = true;
                    break;
                case buttonID_openok:
                    if(FrmGetObjectId(form, index) == pushbuttonID_actload){
                        newdbno = LstGetSelection(dblstptr);
                        CategoryFreeList(qedDB, catlstptr, true, NULL);
                        FrmReturnToForm(formID_main);
                        if(newdbno != noListSelection)
                            FrmUpdateForm(formID_main, newdbno);
                        handled = true;
                        break;
                    }
                case buttonID_opencancel:
                    CategoryFreeList(qedDB, catlstptr, true, NULL);
                    FrmReturnToForm(formID_main);
                    handled = true;
                    break;
            }
        break;
        
        case penDownEvent:
            #ifdef NORMPALM
            if(osversion < 0x03503000)
                SimTapCount(event);             
            #endif
            y = event->screenY;
            x = event->screenX;
            uni2 = event->tapCount;          
            break;

        #ifdef HANDERA
        case lstEnterEvent:
            lee = true;
            break;
        
        case penUpEvent:            
            if(lee){
                lee = false;
                LstEraseList(dblstptr);
                LstDrawList(dblstptr);
            }
            break;
        #endif
                        
        case lstSelectEvent:    
            #ifdef HANDERA
            lee = false;
            #endif                                    
            if((obid=FrmGetObjectId(form, index)) == pushbuttonID_actload){
                if(uni2 == 2)
                    CtlHitControl(GetObjectPointer(buttonID_openok));          
                    
                break;
            }
            
            seltext = LstGetCurrentSelText(dblstptr); 
            
            if((dbid=DmFindDatabase(0, seltext)) == actdbID)
                dbatt = dbAttr;
            else
                DmDatabaseInfo(0, dbid, NULL, &dbatt, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            
            // PopupGetText wants a screen absolute position, x and y are screen relative
            // +38 is not quite correct, correct: rect.topLeft.y + ?
            // works only because top of window is not variable
            y = y - (y-ODDBLTOP) % stdfh + stdfh; 

            switch(obid){
                case pushbuttonID_actren:   // rename file
                    if(DbAttrStatus_Par(dbatt & (dmHdrAttrCopyPrevention | dmHdrAttrReadOnly)))
                        PopupGetText("Rename File:", seltext, 50, y+38, 31, *RenameFileOD, "");
                    break;
                case pushbuttonID_actdel:   // delete file
                    if(DbAttrStatus_Par(dbatt & (dmHdrAttrCopyPrevention | dmHdrAttrReadOnly)))
                        PopupGetText("Delete File:", seltext, 50, y+38, 31, *DeleteFileOD, "");
                    break;
                case pushbuttonID_actcat:   // categorize file
                    if(DbAttrStatus_Par(dbatt & dmHdrAttrReadOnly))
                        PopupDocCatList(seltext, y+1);
                    break;
            }
            break;
        
        case popSelectEvent:    // an item of a popuplist was selected
            SetCategoryFilter(event->data.popSelect.selection);
            // handled = true;     // not true for refreshing popuptriggertext
            break;
        
        case frmOpenEvent:        
            WinGetWindowBounds(&rect);
            y = ppsch - 2*ODTOPY;
            if(y > ODMAXHIGHT) y = ODMAXHIGHT;
            rect.extent.y = y;
            btop = y - ODBARHIGHT;
            WinSetWindowBounds(FrmGetWindowHandle(form), &rect);
            numob = FrmGetNumberOfObjects(form);
            
            for(i=0; i<numob; i++){
                obtype = FrmGetObjectType(form, i);
                
                if(obtype==frmControlObj || obtype==frmBitmapObj){
                    FrmGetObjectPosition(form, i, &x, &y);
                    FrmSetObjectPosition(form, i, x, y+btop);
                }
            }

            LstSetHeight(dblstptr, (btop-ODDBLTOP)/stdfh);
            
            if(!naptr)
                GetDBnames();
            
            PrepOpenDialog(true);
            FrmDrawForm(form);
            CtlSetValue(GetObjectPointer(pushbuttonID_actload), 1);
            WinDrawLine(3, btop+7, 3, btop+25);     // category lines
            WinDrawLine(4, btop+6, 6, btop+6);
            WinDrawLine(4, btop+16, 6, btop+16);
            WinDrawLine(4, btop+26, 6, btop+26);
            WinDrawLine(55, btop+17, 55, btop+25);  // file lines
            WinDrawLine(56, btop+16, 58, btop+16);
            WinDrawLine(56, btop+26, 58, btop+26);
            handled = true;
            break;
        
        case frmCloseEvent:
            CategoryFreeList(qedDB, catlstptr, true, NULL);
            FrmReturnToForm(formID_main);
            handled = true;
            break;        
    }

    return handled;    
}

//*****************************************************************
//
// FormEventGetText
//
//*****************************************************************

#define GTFLDWIDTH      86

static Boolean FormEventGetText(EventPtr event)
{
FormPtr form;
Boolean handled=false;
FieldPtr fptr;
WChar c;
RectangleType rect;

    form = FrmGetActiveForm();
    fptr = GetObjectPointer(fieldID_gettext);

    switch(event->eType){
        case keyDownEvent:          // a key was pressed
            c = event->data.keyDown.chr;
            
            if(c==chrLineFeed || c==chrHorizontalTabulation){      // LF or TAB was sent
                ExitGetText(fptr, c==chrLineFeed);
            }
            else{
                if(qflags&TREOKEYBOARD && c==hsChrPostProcess)
                    HsPostProcessPopupList(false, 0);
                else
                    FldHandleEvent(fptr, event);

                GetTextScroll(fptr);                                              
            }
            
            handled = true;
            break;
        
        case ctlSelectEvent:        // a control button was pressed and released
            switch(event->data.ctlSelect.controlID){
                case buttonID_gtok:
                case buttonID_gtcancel:
                    ExitGetText(fptr, event->data.ctlSelect.controlID==buttonID_gtok);
                    handled = true;
                    break;
            }        
            break;

/*
        case menuEvent:
            switch(event->data.menu.itemID){ 
                case menuitemID_cut:
                    FldCut(fptr);
                    handled = true;
                    break;
                case menuitemID_copy:
                    FldCopy(fptr);
                    handled = true;
                    break;
                case menuitemID_paste:
                    FldPaste(fptr);
                    handled = true;
                    break;
            }
            break;
*/
            
        case frmOpenEvent:
            FldSetTextPtr(GetObjectPointer(fieldID_title), gtinfo->title);
            FldSetMaxChars(fptr, gtinfo->cwidth);

            FldGetBounds(fptr, &rect);  
            rect.extent.x = GTFLDWIDTH;        
            FldSetBounds(fptr, &rect);
            
            FldSetTextHandle(fptr, (MemHandle)gtinfo->texthand);
            FldSetSelection(fptr, 0, FldGetTextLength(fptr));
            
            WinGetWindowBounds(&rect);
            rect.topLeft.x = gtinfo->x;        
            rect.topLeft.y = gtinfo->y - 32;   
            rect.extent.x = GTFLDWIDTH + 8;   

            if(StrLen(gtinfo->checktext1)){
                CtlSetUsable(GetObjectPointer(checkID_get1), true);
                CtlSetLabel(GetObjectPointer(checkID_get1), gtinfo->checktext1);
                CtlSetV(checkID_get1, compbyte==0x02);
            }
            else{
                rect.extent.y = 26 + GSIEXT;            
            }

            WinSetWindowBounds(FrmGetWindowHandle(form), &rect);
            form->window.frameType.bits.cornerDiam = 3;
            form->window.frameType.bits.width = 1;  // standard for modal dialog is 2
            FrmDrawForm(form);
            FrmSetFocus(form, FrmGetObjectIndex(form, fieldID_gettext));            
            // GrfSetState(false, false, true);
            handled = true;
            break;
        
        case frmCloseEvent:
            ExitGetText(fptr, false);
            handled = true;
            break;
    }

    return handled;    
}

//*****************************************************************
//
// FormEventBookmark
//
//*****************************************************************

static Boolean FormEventBookmark(EventPtr event)
{
FormPtr form;
Boolean handled=false;
UInt16 nb, sel, index;
Char *seltext;
ListPtr bsptr;
RectangleType rect;
eventsEnum ev;

    form = FrmGetActiveForm();
    WinGetWindowBounds(&rect);
    index = FrmGetControlGroupSelection(form, groupID_bookgroup);
    ev = event->eType;

    if(ev == keyDownEvent){         // a key was pressed
        ev = ListKeystroke(event->data.keyDown.chr, bmlstptr);
        handled = true;
    }

    switch(ev){
        case ctlSelectEvent:        // a control button was pressed and released
            if(DbAttrStatus_CP() && event->data.ctlSelect.controlID==buttonID_bookadd)
                PopupGetText("Add:", "", BOOKLISTLEFT+2, ppsch-BARHIGHT-GSIEXT-9, 15, *AskAddBookmark, "");
  
            handled = true;
            break;
                    
        #ifdef HANDERA
        case lstEnterEvent:
            lee = true;
            break;
        
        case penUpEvent:            
            if(lee){
                lee = false;
                LstEraseList(bmlstptr);
                LstDrawList(bmlstptr);
            }
            break;
        #endif

        case lstSelectEvent:        // a list item was selected
            #ifdef HANDERA
            lee = false;
            #endif        
            sel = lbooksel = LstGetSelection(bmlstptr);
            seltext = LstGetCurrentSelText(bmlstptr); 
            y = y - y%stdfh + stdfh;
            nb = FrmGetObjectId(form, index);

            if(nb == pushbuttonID_bookjump){
                FreeOldBookMem();
                FrmReturnToForm(formID_main);
                GotoBookmark(sel);           
                break;
            }

            if(DbAttrStatus_CP() && handled==false){    // not for pressed keys (handled=true)
                switch(nb){
                    case pushbuttonID_bookren:
                        PopupGetText("Rename:", seltext, BOOKLISTLEFT+10, y+33+rect.topLeft.y, 15, *AskRenameBookmark, "");
                        break;          
                    case pushbuttonID_bookdel:
                        DeleteBookmark(sel);
                        break;          
                    case pushbuttonID_booksort:
                        bsptr = GetObjectPointer(listID_booksort);
                        LstSetPosition(bsptr, 30, y+1);
                        SortBookmarks(sel, LstPopupList(bsptr));
                        break;          
                }
            }
            
            handled = true;
            break;

        case frmOpenEvent:
            nb = (ppsch-BARHIGHT-14-GSIEXT) / stdfh;    // maximum for screenhight   
            if(nb > MAXBOOK) nb = MAXBOOK;
            if(numbook < nb) nb = numbook;
            if(nb < 3) nb = 3;

            rect.topLeft.y = ppsch - BARHIGHT - nb*stdfh - 14 - GSIEXT; 
            rect.extent.y = nb*stdfh + 12 + GSIEXT;                     
            y = rect.extent.y - 10;                         
            WinSetWindowBounds(FrmGetWindowHandle(form), &rect);
            
            form->window.frameType.bits.cornerDiam = 2;
            form->window.frameType.bits.width = 1;          // standart for modal dialog is 2

            SetObjectPosition(buttonID_bookadd, 1, y);
            SetObjectPosition(bitmapID_bookadd, 1, y);
            
            SetObjectPosition(pushbuttonID_bookjump, 20, y+1);
            SetObjectPosition(bitmapID_bookjump, 19, y);
            SetObjectPosition(pushbuttonID_bookren, 34, y+1);
            SetObjectPosition(bitmapID_bookren, 33, y);
            SetObjectPosition(pushbuttonID_bookdel, 48, y+1);
            SetObjectPosition(bitmapID_bookdel, 47, y);
            SetObjectPosition(pushbuttonID_booksort, 60, y+1);
            SetObjectPosition(bitmapID_booksort, 59, y);
            
            PrepareBookList(true);
            FrmDrawForm(form);
            CtlSetValue(GetObjectPointer(pushbuttonID_bookjump), 1);
            handled = true;
            break;
        
        case penDownEvent:
            x = event->screenX;
            y = event->screenY;

            if(x>=0 && x<BOOKLISTWIDTH && y>=0 && y<rect.extent.y)
                break;

        case frmCloseEvent:
            FreeOldBookMem();
            FrmReturnToForm(formID_main);
            handled = true;
            break;
    }

    return handled;    
}

//*****************************************************************
//
// FormEventSeaRep
//
//*****************************************************************

static Boolean FormEventSeaRep(EventPtr event)
{
FormPtr form;
FieldPtr fpsea, fprep, fptr;
Boolean handled=false;
WChar c;
UInt16 id;
RectangleType rect;

    form = FrmGetActiveForm();
    fpsea = GetObjectPointer(fieldID_seatext);
    fprep = GetObjectPointer(fieldID_reptext);

    switch(event->eType){
        case keyDownEvent:  // a key was pressed
            c = event->data.keyDown.chr;
            
            if(c == chrLineFeed){            
                id = FrmGetObjectId(form, FrmGetFocus(form));   // get id of field has focus
            
                if(srbits&SRF_REPLACE && id==fieldID_seatext){
                    FrmSetFocus(form, FrmGetObjectIndex(form, fieldID_reptext));
                    FldSetSelection(fprep, 0, FldGetTextLength(fprep));
                }
                else{
                    CtlHitControl(GetObjectPointer(buttonID_srnext));
                }
                handled = true;
            }
            else if(c == 9){
                CtlHitControl(GetObjectPointer(buttonID_srcancel));
                handled = true;
            }

            break;
        
        case ctlSelectEvent:        // a control button was pressed and released
            switch(event->data.ctlSelect.controlID){
                case buttonID_srnext:
                    ExitSeaRepForm();
                    Search();                    
                    FrmPopupForm(formID_searep);
                    break;
                case buttonID_srreplace:
                    ExitSeaRepForm();
                    Replace();
                    Search();
                    FrmPopupForm(formID_searep);
                    break;
                case buttonID_srall:
                    ExitSeaRepForm();
                    ReplaceAll();
                    break;
                case buttonID_srcancel:
                    ExitSeaRepForm();
                    break;
                case pushbuttonID_srcase:
                    srbits ^= SRF_CASE;
                    break;
                case pushbuttonID_srword:
                    srbits ^= SRF_WORD;
                    break;
                case pushbuttonID_srtop:
                    srbits ^= SRF_TOP;
                    break;
            }
            handled = true;
            break;
                    
        case menuEvent:
            id = FrmGetObjectId(form, FrmGetFocus(form));   // get id of field has focus
            fptr = GetObjectPointer(id);
            
            switch(event->data.menu.itemID){ 
                case menuitemID_cut:
                    FldCut(fptr);
                    handled = true;
                    break;
                case menuitemID_copy:
                    FldCopy(fptr);
                    handled = true;
                    break;
                case menuitemID_paste:
                    FldPaste(fptr);
                    handled = true;
                    break;
            }
            break;

        case frmOpenEvent:
            WinGetWindowBounds(&rect);

            if(!(srbits & SRF_REPLACE))
                rect.extent.y = 68-22+GSIEXT;          

            rect.topLeft.y = ppsch-rect.extent.y-BARHIGHT-2;         
            WinSetWindowBounds(FrmGetWindowHandle(form), &rect);

            if(!(srbits & SRF_REPLACE)){
                SetObjectPosition(pushbuttonID_srcase, 4, 26+GSIEXT);
                SetObjectPosition(bitmapID_srcase, 3, 25+GSIEXT);
                SetObjectPosition(pushbuttonID_srword, 20, 26+GSIEXT);
                SetObjectPosition(bitmapID_srword, 19, 25+GSIEXT);
                SetObjectPosition(pushbuttonID_srtop, 38, 26+GSIEXT);
                SetObjectPosition(bitmapID_srtop, 37, 25+GSIEXT);
                SetObjectPosition(buttonID_srnext, 2, 34+GSIEXT);
                SetObjectPosition(bitmapID_srnext, 3, 35+GSIEXT);
                SetObjectPosition(buttonID_srcancel, 102, 34+GSIEXT);
                SetObjectPosition(bitmapID_srcancel, 103, 35+GSIEXT);
            }
            
            form->window.frameType.bits.cornerDiam = 2;
            form->window.frameType.bits.width = 1;      // standart for modal dialog is 2

            FldSetTextHandle(fpsea, seathand);
            FldSetSelection(fpsea, 0, FldGetTextLength(fpsea));
            FldSetTextHandle(fprep, repthand);

            FrmDrawForm(form);

            if(srbits & SRF_REPLACE){
                ShowObject(labelID_srrep);
                ShowObject(fieldID_reptext);
            }

            CtlSetValue(GetObjectPointer(pushbuttonID_srcase), srbits&SRF_CASE);
            CtlSetValue(GetObjectPointer(pushbuttonID_srword), srbits&SRF_WORD);
            CtlSetValue(GetObjectPointer(pushbuttonID_srtop), srbits&SRF_TOP);

            if(srbits & SRF_REPFOUND){
                CtlShowControl(GetObjectPointer(buttonID_srreplace));
                ShowObject(bitmapID_srreplace);
                CtlShowControl(GetObjectPointer(buttonID_srall));
                ShowObject(bitmapID_srall);
            }

            FrmSetFocus(form, FrmGetObjectIndex(form, fieldID_seatext));
            handled = true;
            break;
        
        case frmCloseEvent:
            ExitSeaRepForm();
            handled = true;
            break;
    }

    return handled;    
}

//*****************************************************************
//
// ExitSeaRepForm
//
//*****************************************************************

static void ExitSeaRepForm()
{
FieldPtr fpsea, fprep;

    fpsea = GetObjectPointer(fieldID_seatext);
    fprep = GetObjectPointer(fieldID_reptext);

    srbits &= ~SRF_REPFOUND;
    FldSetTextHandle(fpsea, NULL);      // avoid freeing of seathand
    FldSetTextHandle(fprep, NULL);      // avoid freeing of repthand
    FrmReturnToForm(formID_main);
}

//*****************************************************************
//
// FormEventPrefs
//
//*****************************************************************

static Boolean FormEventPrefs(EventPtr event)
{
FormPtr form;
Boolean handled=false;
Int32 spabs, ipabs;

    form = FrmGetActiveForm();

    switch(event->eType){
        case ctlSelectEvent:    // a control button was pressed and released
            switch(event->data.ctlSelect.controlID){
                case buttonID_prefsok:
                    SavePrefs(form);
                case buttonID_prefscancel:
                    MemPtrFree(ptemp);
                    FrmReturnToForm(formID_main);
                    
                    if(event->data.ctlSelect.controlID == buttonID_prefsok){
                        GetSpIpAbs(&spabs, &ipabs);
                        SetFilePrefs();
                        FldScrollH(MAXFW, false);
                        UpdateField(spabs, -1, false);
                        UpdateScrollBar(false);
                    }
                    
                    handled = true;
                    break;
            }
            break;
        case fldEnterEvent:
            // switch(event->data.fldEnter.fieldID)
            //    case fieldID_filename:
            if(ptemp->menuid==menuitemID_docpref && DbAttrStatus_CP_RO())
                PopupGetText("Rename File:", ptemp->name, 50, 68, 31, *ChangePrefsDBN, "");

            handled = true;
            break;
        case frmOpenEvent:
            PrepPrefsForm();
            FrmDrawForm(form);
            handled = true;
            break;
        case frmCloseEvent:
            MemPtrFree(ptemp);
            FrmReturnToForm(formID_main);
            handled = true;
            break;
    }

    return handled;    
}

//*****************************************************************
//
// FormEventRegister
//
//*****************************************************************

static Boolean FormEventRegister(EventPtr event)
{
FormPtr form;
Boolean handled=false;
Char on[dlkUserNameBufSize], rs[10];
UInt16 crc, lid;
static MemHandle reghand;

    form = FrmGetActiveForm();

    switch(event->eType){
        case ctlSelectEvent:    // a control button was pressed and released
            switch(event->data.ctlSelect.controlID){
                case buttonID_register:
                    GetUserName(on);
                    crc = CRC_Calc(on);
                
                    if(StrAToI(MemHandleLock(reghand)) == crc){
                        QedPrefs.regnumber = crc;
                        qflags |= REGFLAG;
                        FrmCustomAlert(alertID_info, "Thank ", "you ", "!");
                    }
                    else{
                        FrmCustomAlert(alertID_info, "Input ", "not ", "valid!");
                    }
                    
                    MemHandleUnlock(reghand);
                
                case buttonID_reglater:
                case buttonID_regok:
            }
            
        case frmCloseEvent:
            FldSetTextHandle(GetObjectPointer(fieldID_regcode), NULL);
            MemHandleFree(reghand);
            FrmReturnToForm(formID_main);
            handled = true;
            break;
        
        case frmOpenEvent:
            GetUserName(on);
            FrmCopyLabel(form, labelID_regowner, on);
            reghand = DmNewHandle(qedDB, 10);
                        
            if(qflags&REGFLAG){                
                lid = labelID_regreg;
                ShowObject(labelID_regreg);
                ShowObject(buttonID_regok);
                StrIToA(rs, QedPrefs.regnumber);
            }
            else{
                lid = labelID_regunreg;
                ShowObject(buttonID_register);
                ShowObject(buttonID_reglater);
                rs[0] = '\0';
            }

            ShowObject(lid);
                  
            if(osversion >= 0x03000000l)
                ((FormLabelType*)GetObjectPointer(lid))->fontID = 7;
                        
            DmStrCopy(MemHandleLock(reghand), 0, rs);
            MemHandleUnlock(reghand);
            FldSetTextHandle(GetObjectPointer(fieldID_regcode), reghand);
            FrmDrawForm(form);
            FrmSetFocus(form, FrmGetObjectIndex(form, fieldID_regcode));
            handled = true;
            break;
    }

    return handled;    
}

#ifdef SONY
//*****************************************************************
//
// DisplayChangeNotification
//
//*****************************************************************

static void DisplayChangeNotefication(SysNotifyParamType *notifyParamsP)
{
FormPtr form;

    if(FrmGetActiveFormID() == formID_main){
        form = FrmGetActiveForm();
        SizeForm(form);
        FrmDrawForm(form);
        SwitchBar(true);
    }
    else{
        sonybits |= DISPLAYCHANGED;
    }
}
#endif

//*****************************************************************
//
// GetTextScroll
//
//*****************************************************************

static void GetTextScroll(FieldPtr fp)
{
Int16 x, y, xx;
RectangleType rect;

    InsPtGetLocation(&x, &y);
    x -= gtinfo->x;             // field starts at x=3
    FldGetBounds(fp, &rect);
    
    if(x > GTFLDWIDTH-4){
        rect.topLeft.x -= 12;   // fp->rect.topLeft.x -= 12;
        rect.extent.x += 12;    // fp->rect.extent.x += 12;
        FldSetBounds(fp, &rect);
        FldDrawField(fp);
    }
    else if(x<15 && (xx=rect.topLeft.x)<3){     // else if(x<15 && (xx=fp->rect.topLeft.x)<3)
        xx += 12;
        rect.topLeft.x = xx;    // fp->rect.topLeft.x = xx;
        FldSetBounds(fp, &rect);
        FldDrawField(fp);
        
        if(xx == 3){
            WinEraseLine(0, 12, 0, 24);
            WinEraseLine(1, 12, 1, 24);
            WinEraseLine(2, 12, 2, 24);
        }
    }
}

//*****************************************************************
//
// ExitGetText
//
//*****************************************************************

static void ExitGetText(FieldPtr fptr, Boolean okflag)
{
void (*p)();

    if(StrLen(gtinfo->checktext1))      // save value of checkmark 1
        gtinfo->check1 = CtlGetV(checkID_get1);
    
    FldSetTextHandle(fptr, NULL);
    FrmReturnToForm(gtinfo->id);

    if(okflag){
        p = gtinfo->funcptr;
        (*p)();            
    }
    
    MemHandleFree(gtinfo->texthand);
    MemPtrFree(gtinfo);
}

//*****************************************************************
//
// CheckField
// called after a key input-, cut-, or paste-operation
//
//*****************************************************************

static void CheckField()
{
static UInt32 tlast;
UInt32 ti;
UInt16 sp, flen;

    if(!(qflags&REGFLAG) && ((nagcount++)&0x3f)==0x3f)
        FrmPopupForm(formID_register);

    sp = FldGetScrollPosition(fldptr);
    flen = FldGetTextLength(fldptr);
    acdbsize = dbsize + flen - lastfsize;

    if(flen-sp<AFTERSP && lrec<lastrec)
        UpdateField(sp, 0, true);
    else if(sp<BEFORESP && frec>STARTREC)
        UpdateField(sp, 0, true);
    else if(flen>MAXFLEN)
        UpdateField(sp, 0, true);

    ti = TimGetTicks();
    
    if(ti-tlast > 40)
        UpdateScrollBar(false);
    
    tlast = ti;
}

//*****************************************************************
//
// FldWriteScroll
//
//*****************************************************************

static void FldWriteScroll()
{
Int16 x, y, xx, pmfw;

    InsPtGetLocation(&x, &y);

    if(x==0){
        QfSetTopLeft(0);
    }
    else if(x < ppscw/5){
        xx = QfGetTopLeft();
        
        if(xx != 0){
            xx += ppscw/5 - x;
            if(abs(xx) < ppscw/5) xx = 0;
            QfSetTopLeft(xx);
        }
    }
    else if(x > 4*ppscw/5){
        xx = QfGetTopLeft();
        pmfw = ppscw-fieldwidth;
        
        if(xx != pmfw){
            xx -= x - 4*ppscw/5;
            if(xx < pmfw) xx = pmfw;
            QfSetTopLeft(xx);
        }
    }
}

//*****************************************************************
//
// FldAutoScroll
//
//*****************************************************************

static void FldAutoScroll()
{
Int16 x, y;
    
    if(fieldwidth > ppscw){
        InsPtGetLocation(&x, &y);   // 0 >= x <= fieldwidth
    
        if(x > 4*ppscw/5)
            FldScrollH(4*ppscw/5 - x, false);
    }
}

//*****************************************************************
//
// FldScrollH
//
//*****************************************************************

static void FldScrollH(Int16 z, Boolean uflag)
{
Int16 xx;

    xx = QfGetTopLeft();
    xx += z;
    if(xx < ppscw-fieldwidth) xx = ppscw-fieldwidth;
    else if(xx > 0) xx = 0;
    QfSetTopLeft(xx);
    
    if(uflag)
        UpdateScrollBar(true);
}

//*****************************************************************
//
// FldScrollV
//
//*****************************************************************

static void FldScrollV(UInt16 n, WinDirectionType d)
{
UInt16 sp, flen;

    FldScrollField(fldptr, n, d);
    sp = FldGetScrollPosition(fldptr);
    flen = FldGetTextLength(fldptr);
    
    if(d==winDown && flen-sp<AFTERSP && lrec<lastrec)
        UpdateField(sp, -1, true);
    else if(d==winUp && sp<BEFORESP && frec>STARTREC)
        UpdateField(sp, -1, true);

    UpdateScrollBar(true);
}

//*****************************************************************
//
// UpdateField
//
//*****************************************************************

static void UpdateField(Int32 sp, Int32 ip, Boolean relative)
{
Int32 spabs, ipabs=-1;
register UInt16 i;
register Int32 sum=0;
Boolean busyactive;

    if(relative){       // relative positioning
        spabs = frabs + sp;
        
        if(ip >= 0)
            if(QfSetGetAttr(GETIPVIS))
                ipabs = frabs + FldGetInsPtPosition(fldptr);
    }
    else{               // absolute positioning            
        spabs = sp;
        ipabs = ip;
    }

    if((UInt32)spabs > acdbsize){   // something is wrong
        spabs = 0;
        ipabs = -1;
    }
    
    busyactive = Field2Recs(false);
    
    frec = 0;
    lrec = lastrec;
    
    for(i=STARTREC; i<=lastrec; i++){    
        sum += rlen[i];
        
        if(!frec && sum>spabs-BEFORESP){
            frec = i;
            frabs = sum - rlen[i];
        }
        if(frec && sum>spabs+AFTERSP){
            lrec = i;
            break;
        }
    }    
    
    Recs2Field();

    FldSetScrollPosition(fldptr, spabs-frabs);
    SetCursor(ipabs);
    
    if(busyactive)
        ShowBusy(NULL);
}

//*****************************************************************
//
// SetCursor
//
//*****************************************************************

static void SetCursor(Int32 ipabs)
{
    if(ipabs!=-1 && ipabs<(Int32)acdbsize && ipabs>=frabs+FldGetScrollPosition(fldptr)){
        FldSetInsPtPosition(fldptr, ipabs-frabs);
        FldGrabFocus(fldptr);
        FldScrollField(fldptr, 1, winDown);
        FldScrollField(fldptr, 1, winUp);
    }
}

//*****************************************************************
//
// InsertRecord
//
//*****************************************************************

static void InsertRecord()
{
MemHandle rlenhand, rechand;
UInt16 index, i;
UInt16 *rlen2;
    
    lastrec++;
    rlenhand = MemHandleNew((lastrec+1)*sizeof(UInt16));
    rlen2 = (UInt16 *)MemHandleLock(rlenhand);
    
    for(i=STARTREC; i<frec; i++)
        rlen2[i] = rlen[i];
    
    rlen2[frec] = 0;
    
    for(i=frec; i<lastrec; i++)
        rlen2[i+1] = rlen[i];
     
    lrec++;
    MemPtrFree((MemPtr)rlen);
    rlen = rlen2;
    
    index = frec;
    rechand = DmNewRecord(docDB, &index, 1);
    DmReleaseRecord(docDB, index, true);  
}

//*****************************************************************
//
// Field2Recs
//
//*****************************************************************

static Boolean Field2Recs(Boolean autobusyoff)   
{
MemHandle memhand;
MemPtr memptr;
UInt16 i;
UInt16 flen;
UInt16 avsize;
UInt32 offset=0;
Boolean fldwasdirty=false;

    if(FldDirty(fldptr)){
        ShowBusy("Compressing...");            
        memhand = (MemHandle)FldGetTextHandle(fldptr);
        
        if(memhand){
            memptr = MemHandleLock(memhand); 
            flen = FldGetTextLength(fldptr);
            
            while((avsize=flen/(lrec-frec+1)) > 4096)
                InsertRecord();
            
            cptrig = flen;
            InitCompProgress();
            
            for(i=frec; i<lrec; i++){
                Mem2Rec(i, memptr+offset, avsize);
                offset += avsize;
            }
            
            Mem2Rec(lrec, memptr+offset, flen-offset);        
            MemPtrUnlock(memptr);
            qflags |= DBCHANGED;
        }
        
        dbsize = acdbsize;
        lastfsize = FldGetTextLength(fldptr);
        FldSetDirty(fldptr, false);
        WriteDocInfo(docDB, actdbID, dbsize, lastrec, compbyte);
        
        if(autobusyoff) 
            ShowBusy(NULL);            
        
        fldwasdirty = true;
    }

    return fldwasdirty;
}

//*****************************************************************
//
// Mem2Rec
//
//*****************************************************************

static void Mem2Rec(UInt16 index, MemPtr mptr, UInt16 size)
{
MemHandle rechand;
MemPtr recptr; 

    rlen[index] = size;

    if(compbyte == 0x02)
        Compress(&mptr, &size);
    
    rechand = DmResizeRecord(docDB, index, size);

    recptr = MemHandleLock(rechand);
    DmWrite(recptr, 0, mptr, size);
    MemPtrUnlock(recptr);
    DmReleaseRecord(docDB, index, false);    

    if(compbyte == 0x02)
        MemPtrFree(mptr);
}

//*****************************************************************
//
// Recs2Field
//
//*****************************************************************

static void Recs2Field()
{
UInt16 i;
UInt16 nsize=0, nsi, offset=0;
MemHandle memhand;
MemPtr memptr;

    for(i=frec; i<=lrec; i++)
        nsize += rlen[i];

    // to save memory the old handle is cleared at first 
    FreeTextHandle();

    memhand = DmNewHandle(docDB, nsize<12000 ? 12000 : nsize+1);  
    ErrFatalDisplayIf(memhand==0, "Low Mem!");
    
    // memhand = DmNewHandle(docDB, nsize+1);  // + 1 for string termination
    memptr = MemHandleLock(memhand);      

    for(i=frec; i<=lrec; i++){
        Rec2Mem(memptr, i, offset, rlen[i]);
        offset += rlen[i];
    }
    
    DmSet(memptr, nsize, 1, 0);         // terminate string
    MemPtrUnlock(memptr); 

    FldSetTextHandle(fldptr, (MemHandle)memhand);

    lastfsize = nsize;                  // for calculation of dbsize        
}

//*****************************************************************
//
// FreeTextHandle
//
//*****************************************************************

static void FreeTextHandle()
{
MemHandle omemhand;

    omemhand = (MemHandle)FldGetTextHandle(fldptr);
    FldSetTextHandle(fldptr, NULL);
    if(omemhand)
        MemHandleFree(omemhand);
}

//*****************************************************************
//
// Rec2Mem
//
//*****************************************************************

static void Rec2Mem(MemPtr mptr, UInt16 index, UInt16 offset, UInt16 size)
{
MemHandle rechand;   
MemPtr recptr; 
UInt8 *zptr;

    rechand = DmGetRecord(docDB, index);
    recptr = MemHandleLock(rechand);
    
    if(compbyte == 0x02){
        zptr = MemPtrNew(size+12);      // oversize
        DeCompress(zptr, recptr, size);
        DmWrite(mptr, offset, zptr, size);
        MemPtrFree(zptr);
    }
    else
        DmWrite(mptr, offset, recptr, size);

    MemPtrUnlock(recptr);
    DmReleaseRecord(docDB, index, false);
}

//*****************************************************************
//
// DeCompress
//
//*****************************************************************

static void DeCompress(UInt8 *zptr, MemPtr fromptr, UInt16 ucsize)
{
UInt8 *end;
register UInt8 *fp, *tp, *ra;
register UInt8 fb, tb;
register UInt16 fw;
    
    tp = zptr;
    fp = (UInt8*)fromptr;
    end = tp + ucsize;

    while(tp < end){
        fb = *fp++;                         // fetch next byte
        
        if(fb & 0x80){                      // any coding?
            if(fb & 0x40){                  // space coding?
                *tp++ = ' ';
                *tp++ = fb & 0x7f;    
            }    
            else{                           // relative coding
                fb <<= 2;
                fw = fb<<6 | *fp++;
                fb = (fw & 0x07) + 3;       // len in fb
                ra = tp - (fw >> 3);        // relative address
                
                while(fb--)
                    *tp++ = *ra++;     
            }
        }
        else if(fb <= 8){                   // raw bytes?
            while(fb--)
                *tp++ = *fp++;            
        }
        else{                               // just a simple character
            *tp++ = (fb=='\r' ? ' ' : fb);  // CR's to spaces
        }        
    }
}

//*****************************************************************
//
// CheckSelection
//
//*****************************************************************

static Boolean CheckSelection()
{
UInt16 startpos, endpos; 
UInt16 flen;
Boolean selflag=false;

    FldGetSelection(fldptr, &startpos, &endpos);
    
    if(endpos!=startpos){
        selflag = true;
        flen = FldGetTextLength(fldptr);
    
        if((flen==endpos && lrec!=lastrec) || (startpos==0 && frec!=STARTREC)){      
            FrmCustomAlert(alertID_info, "maximum selection", " ", " ");
            UpdateScrollBar(true);
        }
    }
    
    return selflag;
}

//*****************************************************************
//
// FetchPrefs
//
//*****************************************************************

static void FetchPrefs()
{
UInt16 size;
Char on[dlkUserNameBufSize];
  
    size = sizeof(QedPrefsType);
    
    if(PrefGetAppPreferences(qedAppID, 1, &QedPrefs, &size, true) == PREFVERSION){
        if(QedPrefs.regnumber!=0 && CRC_Calc(GetUserName(on))==QedPrefs.regnumber)
            qflags |= REGFLAG;
        else
            QedPrefs.regnumber = 0;
    
        QedPrefs.actdbnanu &= 1;
        
        if(QedPrefs.tabwidth > 4) 
                QedPrefs.tabwidth = 4;
    }
    else{
        MemMove(&QedPrefs, &QedDefaultPrefs, sizeof(QedPrefs));
    }
    
    size = 12;
    
    if(PrefGetAppPreferences('psys', 0, on, &size, true)  != noPreferenceFound)
        hideprivate = on[10];   // SystemPreferencesType->hideSecretRecords
    
    DocDefaultAppInfo.dbversion = DBVERSION;
    DocDefaultAppInfo.spabs[0] = DocDefaultAppInfo.spabs[1] = 0;
    DocDefaultAppInfo.ipabs[0] = DocDefaultAppInfo.ipabs[1] = 0;
    MemMove(&DocDefaultAppInfo.dprop, &QedPrefs.dprop, sizeof(dproperties));
}

//*****************************************************************
//
// OpenFirstDB
//
//*****************************************************************

static void OpenFirstDB(void)
{
LocalID lid;
Err err = true;

    if(launchdbid){
        err = OpenDBid(launchdbid, SLEN, APOS);
        launchdbid = 0;
    }
    else if(lid=DmFindDatabase(0, QedPrefs.lastdbname[QedPrefs.actdbnanu])){
        err = OpenDBid(lid, 0, 0);
    }
        
    if(err){    
        DrawBar();
        FrmPopupForm(formID_open);
    }
}

//*****************************************************************
//
// OpenDBid
//
//*****************************************************************

static Err OpenDBid(LocalID dbid, UInt16 slen, Int32 apos)
{
UInt16 dbat;         
UInt32 ds, numrec;
FormPtr form;
UInt8 firstbyte;
UInt8 n;
Int32 ipabs;
        
    DmDatabaseInfo(0, dbid, actdbname, &dbat, NULL, NULL, NULL, NULL, &modnum, NULL, NULL, NULL, NULL);        
    dbAttr = dbat & ~(dmHdrAttrOpen);
    qflags |= READONLYCHANGE;
    
    if(dbat & (dmHdrAttrOpen|dmHdrAttrReadOnly)){           // try to close if open 
        dbat &= ~(dmHdrAttrOpen|dmHdrAttrReadOnly);         // prepare open readwrite
        DmSetDatabaseInfo(0, dbid, NULL, &dbat, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }
                
    if(!(docDB=DmOpenDatabase(0, dbid, dmModeReadWrite))){
        dbAttr |= dmHdrAttrReadOnly;
        qflags &= ~READONLYCHANGE;                          // read only status cannot be changed

        if(!(docDB=DmOpenDatabase(0, dbid, dmModeReadOnly))){
            FrmCustomAlert(alertID_info, "Can't", " open:\n", actdbname);
            return true;
        }
    }

    dbAttrCopy = dbAttr;                                    // copy dbAttr
    FetchDocAppInfo(dbid, &DocAppInfo);
    firstbyte = GetDocInfo();
    DmDatabaseSize(0, dbid, &numrec, NULL, &ds);
        
    if(numrec<=lastrec || numrec<2 || compbyte<1 || compbyte>2 || ds<16 || RecSize(0)<16){
        FrmCustomAlert(alertID_info, "File", " corrupt:\n", actdbname);
        DmCloseDatabase(docDB);
        docDB = 0;
        return true;
    }
        
    actdbID = dbid;    
    numbook = numrec - lastrec - 1;
    lbooksel = 0;
    GetRecLens();
    qflags |= FLOAD;
    SetFilePrefs();
    lasttlx = 0;            // force restore of left/right icons
        
    if(slen == 0){          // restore old cursor position, normal launch
        n = QedPrefs.twinview ? QedPrefs.actdbnanu : 0;                
        ipabs = DocAppInfo.ipabs[n];
        QfSetTopLeft(ipabs<-1 && ipabs>=-MAXFW ? ipabs : 0);
        UpdateField(DocAppInfo.spabs[n], -1, false);
    
        if(editable && ipabs>=0){
            SetCursor(ipabs);
            FldAutoScroll();                    
        }
    }
    else if(slen == -1){    // set cursor to apos, external launch
        QfSetTopLeft(0);
        UpdateField(apos, apos, false);
        FldAutoScroll();                    
    }
    else{                   // set cursor to apos, select slen bytes, GlobalSearch launch
        QfSetTopLeft(0);
        UpdateField(apos, apos+slen, false);
        apos -= frabs;                  // convert to relative position
        FldSetSelection(fldptr, apos, apos+slen);
        FldAutoScroll();                    
    }
        
    UpdateScrollBar(false);
    qflags &= ~DBCHANGED;   // not changed yet
    
    // set focus for softkeyboard coming up easy
    form = FrmGetActiveForm();  
    FrmSetFocus(form, FrmGetObjectIndex(form, fieldID_main));
    
    return false;
}

//*****************************************************************
//
// HandleDocAppInfo
//
//*****************************************************************

static MemPtr HandleDocAppInfo(LocalID dbid)
{
LocalID appInfoID;
Int32 version;
Boolean nflag=false;
MemHandle infohand;
DocAppInfoPtr infoptr;

    DmDatabaseInfo(0, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appInfoID, NULL, NULL, NULL);
    
    if(appInfoID){
        infohand = MemLocalIDToGlobal(appInfoID, 0);
        infoptr = MemHandleLock(infohand);
        version = infoptr->dbversion;
        MemHandleUnlock(infohand);

        if(version != DBVERSION){
            nflag = true;
            
            if(MemHandleSize(infohand) != sizeof(DocAppInfo))
                ErrFatalDisplayIf(MemHandleResize(infohand, sizeof(DocAppInfo)), "Cannot resize handle!");
        }
    }
    else{
        infohand = DmNewHandle(qedDB, sizeof(DocAppInfo));
        appInfoID = MemHandleToLocalID(infohand);
        nflag = true;
    }
    
    infoptr = MemHandleLock(infohand);

    if(nflag){
        DmSetDatabaseInfo(0, dbid, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appInfoID, NULL, NULL, NULL);
        DmWrite(infoptr, 0, &DocDefaultAppInfo, sizeof(DocAppInfo));
    }
    
    return infoptr;
}

//*****************************************************************
//
// GenNewDBname
//
//*****************************************************************

static void GetNewDBName(Char *name)
{
UInt16 nc=0;
LocalID dbid;

    do{
        StrCopy(name, "untitled ");
        StrIToA(name + StrLen(name), ++nc);
        dbid = DmFindDatabase(0, name);
    }while(dbid);
}

//*****************************************************************
//
// OpenNewEmptyDB
//
//*****************************************************************

static void OpenNewEmptyDB()
{
MemPtr vp;
Char newname[dmDBNameLength];
DmOpenRef oref;
MemHandle rechand;
MemPtr memptr;
UInt16 index = 1;
LocalID dbid;
Boolean edflag;
    
    vp = MemHandleLock(gtinfo->texthand);
    StrCopy(newname, vp);
    MemPtrUnlock(vp);
    
    dbid = DmFindDatabase(0, newname);
    
    if(dbid){
        FrmCustomAlert(alertID_info, "'", newname, "'\nalready exists!");        
    }
    else{
        DmCreateDatabase(0, newname, docAppID, docDBType, false);
        dbid = DmFindDatabase(0, newname);
        
        if(dbid){
            CloseDB(SAVE_FIELD|SAVE_SPIP|AUTOBUSYOFF);
            
            oref = DmOpenDatabase(0, dbid, dmModeReadWrite);
            WriteDocInfo(oref, dbid, 1, 1, gtinfo->check1 ? 0x02 : 0x01); 
            rechand = DmNewRecord(oref, &index, 1);
            memptr = MemHandleLock(rechand);
            DmSet(memptr, 0, 1, 32); 
            MemPtrUnlock(memptr); 
            DmReleaseRecord(oref, index, false);
            DmCloseDatabase(oref);
            GetDBnames();                               // for next open dialog
            edflag = DocDefaultAppInfo.dprop.editable;  // save flag 
            DocDefaultAppInfo.dprop.editable = true;    // new opened files are editable
            OpenDBid(dbid, 0, 0);
            DocDefaultAppInfo.dprop.editable = edflag;  // restore flag
            dbAttr |= dmHdrAttrBackup;                  // set backup bit 
        }
    }
}

//*****************************************************************
//
// PrepOpenDialog
//
//*****************************************************************

static void PrepOpenDialog(Boolean selrec)
{
Int16 sdbno=-1;
Int16 lipos;
UInt16 i;
RectangleType rect;
    
    LstSetListChoices(dblstptr, npaptr, numdb);
    
    if(qflags&FLOAD && selrec){
        for(i=0; i<numdb; i++){
            if(!StrCompare(LstGetSelectionText(dblstptr, i), actdbname)){
                lipos = i - 3;
                LstSetTopItem(dblstptr, lipos<0 ? 0 : lipos);
                LstSetSelection(dblstptr, i);
                break;
            }
        }
    }
    
    CategoryCreateList(qedDB, catlstptr, QedPrefs.category, true, true, NULL, NULL, true);        
    i = LstGetNumberOfItems(catlstptr);
    if(i > 8) i = 8; 
    lipos = LstGetSelection(catlstptr);
    CtlSetLabel(GetObjectPointer(popuptriggerID_catfilt), LstGetCurrentSelText(catlstptr)); 
    LstSetHeight(catlstptr, i);
    lipos -= 3;
    LstSetTopItem(catlstptr, lipos<0 ? 0 : lipos);
    WinGetWindowBounds(&rect);
    LstSetPosition(catlstptr, 10, rect.extent.y-ODBARHIGHT-1-i*stdfh);
}

//*****************************************************************
//
// CloseDB
//
//*****************************************************************

static void CloseDB(UInt8 flags)
{
    if(docDB){
        if(flags & SAVE_SPIP) 
            SaveSpIp();

        DocAppInfo.dprop.editable = editable;
        UpdateDocAppInfo(actdbID, &DocAppInfo);

        if(flags & SAVE_FIELD)
            Field2Recs(flags & AUTOBUSYOFF);
        
        DmCloseDatabase(docDB);
        
        if(qflags&DBCHANGED)
            modnum++;          

        if(qflags&DBCHANGED || dbAttr!=dbAttrCopy)
            DmSetDatabaseInfo(0, actdbID, NULL, &dbAttr, NULL, NULL, NULL, NULL, &modnum, NULL, NULL, NULL, NULL);
        
        docDB = 0;
        actdbID = 0;

        FreeTextHandle();
        FldEraseField(fldptr);
        QfSetGetAttr(false);        // prevent writing if no file is loaded
        actdbname[0] = '\0';
        qflags &= ~FLOAD;
        sbposold = 0;               // force redraw of scroll bar for next file

        MemPtrFree((MemPtr)rlen);
        MemHeapCompact(0);
    }
}

//*****************************************************************
//
// SaveSpIp
//
//*****************************************************************

static void SaveSpIp(void)
{
UInt8 n;
Int32 ipabs;
Int16 xx;

    n = QedPrefs.twinview ? QedPrefs.actdbnanu : 0;                        
    GetSpIpAbs(&DocAppInfo.spabs[n], &ipabs);
    
    if(!editable)
        ipabs = -1;
    
    if(ipabs==-1 && (xx=QfGetTopLeft()))
        ipabs = xx;
    
    DocAppInfo.ipabs[n] = ipabs;
}

//*****************************************************************
//
// WriteDocInfo
//
//*****************************************************************

static void WriteDocInfo(DmOpenRef dmor, LocalID dbid, UInt32 size, UInt16 recs, UInt8 comp)
{
MemHandle rechand;   
MemPtr recptr; 
UInt32 nrec;
UInt16 index=0;

    DmDatabaseSize(0, dbid, &nrec, NULL, NULL);
    
    if(!nrec){
        DmNewRecord(dmor, &index, 16);
        DmReleaseRecord(dmor, index, false);  
    }

    rechand = DmGetRecord(dmor, 0);
    recptr = MemHandleLock(rechand);
    
    DmWrite(recptr, 0, "\00", 1);                   // 0: 0
    DmWrite(recptr, 1, &comp, 1);                   // 1: compressbyte
    DmWrite(recptr, 2, "\00\00", 2);                // 2-3: 0
    DmWrite(recptr, 4, &size, 4);                   // 4-7: uncompressed len
    DmWrite(recptr, 8, &recs, 2);                   // 8-9: num of records
    DmWrite(recptr, 10, "\x10\00\00\00\00\00", 6);  // ?

    MemPtrUnlock(recptr);
    DmReleaseRecord(dmor, 0, false);
}

//*****************************************************************
// GetDBnames
// frees the dbnames-array and the dbna-pointer-array if necessary
// checks out how many databases are available
// generates a new empty database if no one is available
// allocates mem for the dbnames-array and fills it
// allocates mem for the dbna-pointer-array and fills it
//*****************************************************************

#define NASTLEN 300
#define DARLEN 80

static void GetDBnames()
{
MemHandle nh;
UInt16 ndb;
Char *np, *np2, *np3;
UInt32 nlsum, csize, nls, i;
LocalID apinid;
QedCatAppInfoPtr apinptr;
UInt16 delcat;
       
    FreeOldNameMem();
        
    csize = NASTLEN;
    nh = MemHandleNew(csize);    
    np = np2 = (Char*)MemHandleLock(nh);  
    nlsum = 0;
    numdb = 0;

    apinid = DmGetAppInfoID(qedDB);
    apinptr = MemLocalIDToLockedPtr(apinid, 0);
    delcat = apinptr->renamedCategories;
    MemPtrUnlock(apinptr);

    while(FetchDBname(numdb==0, np2, delcat)){
        nlsum += 32;
        np2 += 32;
        
        if(csize-nlsum < 32){
            csize += NASTLEN;
            MemHandleUnlock(nh);
            MemHandleResize(nh, csize);   
            np = np2 = (Char*)MemHandleLock(nh);
            np2 += nlsum;
        }
        numdb++;
    }
    
    if(numdb){
        nls = nlsum;
        ndb = numdb;
    }
    else{
        nls = 1;
        ndb = 1;
        np[0] = 0;      // empty name string
    }
    
    if(numdb > 1)
        SysQSort(np, numdb, 32, *comparF, NULL);

    np3 = np;           // strip unused space                       
    
    for(i=0; i<nls; i+=32){
        np2 = np + i;
    
        while((*np3++=*np2++) != '\0')
            ;           
    }
    
    nls = np3 - np; 
    nh = DmNewHandle(qedDB, nls);
    naptr = MemHandleLock(nh);
    DmWrite(naptr, 0, np, nls);     // copy to storage mem
    MemPtrFree(np);                 // free heap mem

    npaptr = (Char**)MemHandleLock(SysFormPointerArrayToStrings(naptr, ndb));
}

//*****************************************************************
//
// FreeOldNameMem
//
//*****************************************************************

static void FreeOldNameMem()
{
    if(naptr)
        MemPtrFree((MemPtr)naptr);
    
    if(npaptr)
        MemPtrFree((MemPtr)npaptr);    
    
    naptr = 0;
    npaptr = 0;
}

//*****************************************************************
// from DataPrv.h only for OS < 5

typedef struct{
    LocalID nextRecordListID;           // local chunkID of next list
    UInt16  numRecords;                 // number of records in this list
    UInt32  firstEntry_ID;              // array of Record/Rsrc entries 
    UInt8   attrib;                     // starts here
    }RecordListType;
typedef RecordListType* RecordListPtr;

typedef struct{
    UInt8   name[dmDBNameLength];       // name of database
    UInt16  attributes;                 // database attributes
    UInt16  version;                    // version of database
    
    UInt32  creationDate;               // creation date of database
    UInt32  modificationDate;           // latest modification date
    UInt32  lastBackupDate;             // latest backup date
    UInt32  modificationNumber;         // modification number of database
    
    LocalID appInfoID;                  // application specific info
    LocalID sortInfoID;                 // app specific sorting info
    
    UInt32  type;                       // database type
    UInt32  creator;                    // database creator 

    UInt32  uniqueIDSeed;               // used to generate unique IDs.
                                        // Note that only the low order
                                        // 3 bytes of this is used (in
                                        // RecordEntryType.uniqueID).
                                        // We are keeping 4 bytes for 
                                        // alignment purposes.

    RecordListType recordList;          // first record list
}DatabaseHdrType;       

typedef DatabaseHdrType* DatabaseHdrPtr;

//*****************************************************************
// FetchDBname
// given the first-flag and a Char* and using the dbID it
// copys the namestring of the actual database to the Char* and
// returns the len of the namestring incl. the termination zero
//*****************************************************************

static Boolean FetchDBname(Boolean first, Char *name, UInt16 delcat)
{
static UInt16 ndb, dbi;
static UInt8 actcat;
LocalID ldbid;
DatabaseHdrPtr register dbhptr;
UInt8 dcat;
UInt8 recatt;
Boolean priv;
UInt32 type, creator;

    if(first){
        ndb = DmNumDatabases(0);
        dbi = 0;
        actcat = QedPrefs.category;
    }
    
    if(osversion < 0x05000000){
        while(dbi < ndb){
            ldbid = DmGetDatabase(0, dbi++);
            dbhptr = MemLocalIDToLockedPtr(ldbid, 0);
    
            if(dbhptr->type==docDBType && dbhptr->creator==docAppID){
                recatt = dbhptr->recordList.attrib;
                dcat = recatt&dmRecAttrCategoryMask;
                priv = recatt&dmRecAttrSecret;
    
                if((actcat==dcat || actcat==ALL || (actcat==UNFILED && delcat&(1<<dcat))) && !(priv && hideprivate)){
                    StrCopy(name, dbhptr->name);
                    MemPtrUnlock(dbhptr);
                    return true;
                }
            }
    
            MemPtrUnlock(dbhptr);
        }
    }
    else{
        while(dbi < ndb){
            ldbid = DmGetDatabase(0, dbi++);
            DmDatabaseInfo(0, ldbid, name, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &type, &creator);
    
            if(type==docDBType && creator==docAppID){
                recatt = DocAttribute(ldbid, NULL);
                dcat = recatt&dmRecAttrCategoryMask;
                priv = recatt&dmRecAttrSecret;
                
                if((actcat==dcat || actcat==ALL || (actcat==UNFILED && delcat&(1<<dcat))) && !(priv && hideprivate))
                    return true;
            }
        }
    }
    
    return false;
}

//*****************************************************************
//
// comparF  
// returns  > 0 if A > B
//          < 0 if A < B
//            0 if A = B
//
//*****************************************************************

Int16 comparF(MemPtr a, MemPtr b, Int32 other)
{
    return(StrCompare(a, b));
}

//*****************************************************************
// GetRecLens
// allocates mem for new rlen-array
// fills rlen-array with size of every record in actual database
//*****************************************************************

static void GetRecLens()
{
MemHandle rlenhand;
UInt16 i;
UInt32 sum = 0;

    ShowBusy("Loading...");
    
    rlenhand = MemHandleNew((lastrec+1)*sizeof(UInt16));
    rlen = MemHandleLock(rlenhand);     
 
    for(i=STARTREC; i<=lastrec; i++){
        rlen[i] = RecSize(i);
        sum += rlen[i];
    }
    
    dbsize = acdbsize = sum;
    ShowBusy(NULL);
}

//*****************************************************************
// RecSize
// given a record-index it
// returns the records size 
//*****************************************************************

static UInt16 RecSize(UInt16 in)
{
LocalID chunkID;
MemHandle chunkhand;
MemPtr chunkptr;
UInt16 size;
    
    // till version 2.10
    // DmRecordInfo(docDB, in, NULL, NULL, &chunkID);
    // chunkhand = (MemHandle)MemLocalIDToGlobal(chunkID, 0);

    chunkhand = DmQueryRecord(docDB, in);
    chunkptr = MemHandleLock(chunkhand);
    size = MemHandleSize(chunkhand);

    if(compbyte==0x02 && in)
        size = GetUncompSize(chunkptr, size);
    
    MemHandleUnlock(chunkhand);
    return size;
}

//*****************************************************************
// 
// GetUncompSize
// 
//*****************************************************************

static UInt16 GetUncompSize(MemPtr mptr, UInt16 compsize)
{
UInt8 *end;
register UInt8 *bp;
register UInt8 b;
register UInt16 s=0;
    
    bp = (UInt8*)mptr;
    end = bp + compsize;

    while(bp < end){
        b = *bp++;          // fetch next byte
        if(b & 0x80){       // any coding?
            if(b & 0x40){   // space coding?
                s += 2;
            }    
            else{                           // relative coding
                s += (*bp++ & 0x07) + 3;    // len in next byte
            }
        }
        else if(b <= 8){    // raw bytes?
            s += b;
            bp += b;    
        }
        else{               // just a simple character
            s++;
        }        
    }

    return s;
}

//*****************************************************************
// 
// UpdateScrollBar
// 
//*****************************************************************

static void UpdateScrollBar(Boolean posonly)
{
UInt16 flen;
UInt8 newpos, newsize;
UInt16 scrollPos, textHeight, fieldHeight;
Boolean sflag=false;
UInt32 fw, xp;

    if(QedPrefs.baron){
        if(qflags&HSCROLL){
            fw = (UInt32)fieldwidth;
            xp = -QfGetTopLeft();
            
            if(posonly)
                newsize = sbsizeold;
            else 
                newsize = ppscw * SCROLLBARWIDTH / fw;
            
            newpos = xp * SCROLLBARWIDTH / fw;
            if(xp == fw-ppscw) newpos++;
        }
        else{
            if(posonly && acdbsize>0){
                newsize = sbsizeold;
                sflag = true;
            }
            else{
                FldGetScrollValues(fldptr, &scrollPos, &textHeight, &fieldHeight);
        
                if(textHeight > fieldHeight){
                    flen = FldGetTextLength(fldptr);
                    sbscrs = (UInt32)flen * fieldHeight / textHeight;
                    newsize = (UInt32)sbscrs * SCROLLBARWIDTH / acdbsize;
                    if(newsize < MINSBCARSIZE) newsize = MINSBCARSIZE;
                    sflag = true;
                }
            }
                            
            if(sflag){           
                newpos = (UInt32)(frabs + FldGetScrollPosition(fldptr)) * (SCROLLBARWIDTH-newsize) / (acdbsize-sbscrs);
            }
            else{
                newsize = SCROLLBARWIDTH;
                newpos = 0;
            }
        }

        if(newpos > SCROLLBARWIDTH-newsize) newpos = SCROLLBARWIDTH-newsize;
        newpos += SCROLLBARLEFT;

        if(sbposold!=newpos || sbsizeold!=newsize){
            DrawScrollBar(newpos, newsize);
            sbposold = newpos;
            sbsizeold = newsize;
        }
    }
}

//*****************************************************************
// 
// JumpScrollBarPosition
// 
//*****************************************************************

static void JumpScrollBarPosition(Int16 xpos)
{
Int32 spabs;
UInt8 newpos;
Int16 ScreenX, ScreenY, spx;
Boolean PenDown, bflag=false;

    if(!(uni1&SCROLLBARSLIDE)){
        if(xpos<sbposold || xpos>sbposold+sbsizeold)
            uni2 = 0;
        else 
            uni2 = xpos - sbposold;

        uni1 |= SCROLLBARSLIDE;
    }
    
    newpos = xpos - uni2;

    if(newpos < SCROLLBARLEFT){ 
        newpos = SCROLLBARLEFT;
        bflag = true;
    }
    else if(newpos > SCROLLBARLEFT+SCROLLBARWIDTH-sbsizeold){
        newpos = SCROLLBARLEFT+SCROLLBARWIDTH-sbsizeold;
        bflag = true;
    }
    
    if(sbsizeold == SCROLLBARWIDTH)
        bflag = false;
    
    if(sbposold!=newpos || bflag){
        DrawScrollBar(newpos, sbsizeold);
        sbposold = newpos;
        EvtGetPen(&ScreenX, &ScreenY, &PenDown);
        
        if(ScreenX == xpos){
            if(qflags&HSCROLL){
                spx = -QfGetTopLeft() - (Int32)fieldwidth * (newpos-SCROLLBARLEFT) / SCROLLBARWIDTH;
                FldScrollH(spx, false);   
            }
            else{
                if(newpos != SCROLLBARLEFT+SCROLLBARWIDTH-sbsizeold) 
                    spabs = (acdbsize-sbscrs) * (newpos-SCROLLBARLEFT) / (SCROLLBARWIDTH-sbsizeold);
                else
                    spabs = acdbsize;
                
                UpdateField(spabs, -1, false);
            }
        }            
    }
}

//*****************************************************************
// 
// DrawScrollBar
// 
//*****************************************************************

static void DrawScrollBar(UInt8 carpos, UInt8 carsize)
{
RectangleType r;
Coord top;
IndexedColorType prevcol;

    top = ppsch-BARHIGHT+ICONROW1;              // B&W bitmaps are drawn with white as the backcolor
    r.topLeft.x = carpos;                       // and black with the forecolor
    r.topLeft.y = top;                          // so a backerase isn't necessary
    r.extent.x = carsize;
    r.extent.y = SCROLLBARHIGHT;

    if(qflags&COLORMODE)                        // WinSetBackColor is done before DrawBarBitmap to 
        prevcol = WinSetBackColor(barbackcol);  // save a second WinSetBackColor with backerase

    DrawBarBitmap(SCROLLBAR_ID-FIRSTMCBID, 0, false);
    WinDrawRectangle(&r, 0);

    WinEraseLine(carpos-1, top, carpos-1, top+SCROLLBARHIGHT-1);
    WinEraseLine(carpos+carsize, top, carpos+carsize, top+SCROLLBARHIGHT-1);

    if(qflags&COLORMODE)
        WinSetBackColor(prevcol);

/*
    #ifdef SONY
    top = top<<1;
    carpos = carpos<<1;
    carsize = carsize<<1;
    HRWinEraseLine(HRrefNum, carpos-1, top, carpos-1, top+(SCROLLBARHIGHT<<1));
    HRWinEraseLine(HRrefNum, carpos+carsize, top, carpos+carsize, top+(SCROLLBARHIGHT<<1));
    #else
    WinEraseLine(carpos-1, top, carpos-1, top+SCROLLBARHIGHT);
    WinEraseLine(carpos+carsize, top, carpos+carsize, top+SCROLLBARHIGHT);
    #endif
*/
}

//****************************************************************
// 
// RenameFileOD (from Opendialog)
// 
//*****************************************************************

static void RenameFileOD(void)
{
MemPtr vp;

    vp = MemHandleLock(gtinfo->texthand);
    RenameFile(LstGetCurrentSelText(dblstptr), vp);
    MemPtrUnlock(vp);

    RefreshOpenDialog();  
}

//****************************************************************
// 
// RenameFileME (from Menu)
// 
//*****************************************************************

static void RenameFileME(void)
{
MemPtr vp;

    vp = MemHandleLock(gtinfo->texthand);
    RenameFile(actdbname, vp);
    MemPtrUnlock(vp);

    GetDBnames();
}

//****************************************************************
// 
// RenameFile
// 
//*****************************************************************

static void RenameFile(Char *oldname, Char *newname)
{
LocalID dbidold, dbidnew;

    dbidold = DmFindDatabase(0, oldname);
    dbidnew = DmFindDatabase(0, newname);

    if(dbidnew){
        FrmCustomAlert(alertID_info, "'", newname, "'\nalready exists!");        
    }
    else{    
        if(dbidold){
            DmSetDatabaseInfo(0, dbidold, newname, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            
            if(dbidold == actdbID)
                StrCopy(actdbname, newname);
        }
    }
}

//*****************************************************************
// 
// DeleteFileOD (from Opendialog)
// 
//*****************************************************************

static void DeleteFileOD(void)
{
LocalID dbid;
MemPtr vp;

    vp = MemHandleLock(gtinfo->texthand);
    dbid = DmFindDatabase(0, vp);
    MemPtrUnlock(vp);
    
    if(dbid){
        if(dbid == actdbID){
            CategoryFreeList(qedDB, catlstptr, true, NULL);
            FrmReturnToForm(formID_main);
            CloseDB(SAVE_SPIP);
            DmDeleteDatabase(0, dbid);
            GetDBnames();
            FrmPopupForm(formID_open);
        }      
        else{
            DmDeleteDatabase(0, dbid);
            RefreshOpenDialog();  
        }
    }
}

//*****************************************************************
// 
// DeleteFileME (from Menu)
// 
//*****************************************************************

static void DeleteFileME(void)
{
LocalID dbid;
MemPtr vp;

    vp = MemHandleLock(gtinfo->texthand);
    dbid = DmFindDatabase(0, vp);
    MemPtrUnlock(vp);

    if(dbid){
        CloseDB(SAVE_SPIP);
        DmDeleteDatabase(0, dbid);
        GetDBnames();
    }
}

//*****************************************************************
// 
// AskNewFile
// 
//*****************************************************************

static void AskNewFile()
{
Char name[dmDBNameLength];

    GetNewDBName(name);
    PopupGetText("New File:", name, GTLEFT, ppsch-BARHIGHT-GSIEXT-8, 31, *OpenNewEmptyDB, "Compress");
}

//*****************************************************************
// 
// AskBookChars
// 
//*****************************************************************

static void AskBookChars()
{
MemPtr vp;
    
    vp = MemHandleLock(gtinfo->texthand);
 
    if(StrLen(vp))
        StrCopy(QedPrefs.bookchars, vp);
    
    MemPtrUnlock(vp);
}

//*****************************************************************
// 
// GetDocInfo
// 
//*****************************************************************

static UInt8 GetDocInfo()
{
MemHandle rechand;   
MemPtr recptr; 
UInt8 firstbyte;
   
    rechand = DmGetRecord(docDB, 0);
    recptr = MemHandleLock(rechand);
    
    firstbyte = *(UInt8*)(recptr);
    compbyte = *(UInt8*)(recptr + 1);
    // files >64 k become corrupted by AportisDoc
    // dbsize = acdbsize = *(UInt32*)(recptr + 4);
    lastrec = *(UInt16*)(recptr + 8);

    MemPtrUnlock(recptr);
    DmReleaseRecord(docDB, 0, false);

    return firstbyte;
}

//*****************************************************************
// 
// ShowDBinfo
// 
//*****************************************************************

static void ShowDBinfo(void)
{
Char t1[130], zwi[16];
UInt16 i;
MemHandle memhand;
MemPtr memptr;
UInt32 words=0;
Char c, lastc;
Char *p, *end;
UInt32 dbs;

#ifdef xinfo
UInt32 hsize, free, max;
Char t2[100];

    MemHeapCompact(0);
    hsize = MemHeapSize(0);
    MemHeapFreeBytes(MemHeapID(0, 0), &free, &max);
#endif

    Field2Recs(false);          // save if necessary

    ShowBusy("Counting words...");

    for(i=STARTREC; i<=lastrec; i++){
        memhand = DmNewHandle(qedDB, rlen[i]+12);               // oversize
        ErrFatalDisplayIf(memhand==0, "Low Mem!");
        memptr = MemHandleLock(memhand);      
        Rec2Mem(memptr, i, 0, rlen[i]);
        p = memptr;
        end = memptr + rlen[i];
        lastc = ' ';
        
        while(p < end){
            c = *p++;
            
            if(c<=' ' && lastc>' ')
                words++;
        
            lastc = c;
        }
        
        MemPtrFree(memptr);
    }

    if(lastc > ' ')
        words++;

    // prepare database infos
    StrCopy(t1, "'");                                           //  1
    StrCat(t1, actdbname);                                      // 34
    StrCat(t1, "'\n");                                          // 36  
    
    StrCat(t1, StrIToA(zwi, words));                            // 48
    StrCat(t1, " words\n");                                     // 55
         
    StrCat(t1, StrIToA(zwi, acdbsize));                         // 67
    StrCat(t1, " bytes\n");                                     // 74
    
    if(compbyte==0x02){
        DmDatabaseSize(0, actdbID, NULL, NULL, &dbs);                         
        StrCat(t1, StrIToA(zwi, dbs));                          // 86
        StrCat(t1, " compressed\n");                            // 98                     
    }
    
    if(dbAttr & dmHdrAttrReadOnly){
        StrCat(t1, "Read Only");
    }
    else{
        StrCat(t1, (qflags&DBCHANGED || FldDirty(fldptr)) ? "" : "un");    // 100
        StrCat(t1, "modified");                                     // 108
    
        if(dbAttr & dmHdrAttrCopyPrevention)
            StrCat(t1, "\nVFS File");
    }
    
#ifdef xinfo
    // prepare extended info
    StrCopy(t2, "\n\nHeap: ");
    StrCat(t2, StrIToA(zwi, hsize));
    StrCat(t2, "\nFree:   ");
    StrCat(t2, StrIToA(zwi, free));
    StrCat(t2, "\nMax:   ");
    StrCat(t2, StrIToA(zwi, max));
#endif
    
    ShowBusy(NULL);

    FldReleaseFocus(fldptr);    // avoid cursorfragment in alertwindow

#ifdef xinfo
    FrmCustomAlert(alertID_info, t1, t2, " ");
#else
    FrmCustomAlert(alertID_info, t1, " ", " ");
#endif

    FldGrabFocus(fldptr);
}

//*****************************************************************
// 
// ShowBusy
// 
//*****************************************************************

static void ShowBusy(Char *title)
{   
RectangleType rect;
FontID fid;

    if(!(srbits & (SRF_REPALL | SRF_GLOBSEA))){ // no infotexts while global find or replace all
        if(title){
            MenuEraseStatus(NULL);
            rect.topLeft.x = 0;
            rect.topLeft.y = ppsch-BARHIGHT;
            rect.extent.x = ppscw;
            rect.extent.y = BARHIGHT;
            WinEraseRectangle(&rect, 0);
            
            if(StrNCompare(title, "Com", 3)){   // if not Compress 
                rect.topLeft.y = ppsch-CPHIGHT;
                rect.extent.y = CPHIGHT;
                WinDrawRectangle(&rect, 0);
            }

            fid = FntGetFont();
            FntSetFont(stdFont);
            WinInvertChars(title, StrLen(title), CPPOSX+3, ppsch-CPHIGHT+2);
            FntSetFont(fid);
        }
        else{
            SwitchBar(qflags&FLOAD);
        }
    }
}

//*****************************************************************
// 
// Compress
// 
//*****************************************************************

static void Compress(MemPtr *memptr, UInt16 *size)
{
register UInt8 fb, fb2;
register UInt8 *fp, *sp;
register UInt16 tw;
register UInt8 c, cmerk;
register UInt8 *fp2, *sp2, *end, *tp;
UInt8 *start, *spmerk, *tstart;
UInt16 s;

    s = *size;    
    fp = (UInt8*)*memptr;
    *memptr = MemPtrNew(s+200);

    tp = tstart = (UInt8*)*memptr;
    start = fp;
    end = fp + s;
    
    while(fp < end){
        sp = fp - 2047;
        
        if(sp < start)
            sp = start;
        
        cmerk = 1;
        fb = *fp;
        
        while(sp < fp){
            if(*sp++ == fb){
                c = 0;
                fp2 = fp; fp2++;
                sp2 = sp;
                
                while(*sp2==*fp2 && c<9 && fp2<end){
                    sp2++;
                    fp2++;
                    c++;
                }
                if(c > cmerk){
                    cmerk = c;
                    spmerk = sp;
                    
                    if(c == 9)
                        break;
                }
            }
        }
        
        fp++;
        cprest++;               // progress bar
        
        if(cmerk >= 2){         // relative coding
            tw = 0x8000 | fp-spmerk<<3 | cmerk-2;
            *tp++ = tw>>8;
            *tp++ = tw;
            fp += cmerk;
            cprest += cmerk;    // progress bar
        }
        else if(fb==' ' && (fb2=*fp)>='@' && fb2<='z' && fp<end){
            *tp++ = 0x80 | fb2; // space coding
            fp++;
            cprest++;           // progress bar
        }
        else if(fb <= 'z'){     // no coding
            *tp++ = fb;
        }                
        else{                   // raw bytes >'z'
            sp2 = tp++;
            *tp++ = fb;
            c = 1;
            
            while((fb=*fp)>'z' && c<8 && fp<end){
                fp++;
                cprest++;       // progress bar
                *tp++ = fb;
                c++;
            }
            
            *sp2 = c;
        }
        
        if(cprest >= cptrig){   // progress bar
            WinInvertLine(cpx, ppsch-CPHIGHT, cpx, ppsch-1);
            cpx++;
            cprest -= cptrig;
        }
    }
    
    *size = tp - tstart;
}

//*****************************************************************
// 
// SetFilePrefs
// 
//*****************************************************************

static void SetFilePrefs(void)
{
MemHandle fontHandle;
FontID newfont;
FieldAttrType attr;
RectangleType rect;

    FldGetAttributes(fldptr, &attr);
    attr.underlined = DocAppInfo.dprop.ruled;
    editable = DocAppInfo.dprop.editable;
            
    attr.usable = true;                 // set field usable again after CloseDB           
    newfont = DocAppInfo.dprop.font;
    
    if(newfont>7 || (newfont==REPFONT1 && !repfont[0].sysptr) || (newfont==REPFONT2 && !repfont[1].sysptr))
        newfont = stdFont;    
    
    FldSetFont(fldptr, newfont);
    FntSetFont(newfont);

    // for new Handera fonts
    // FldSetFont(fldptr, VgaBaseToVgaFont(newfont));
    // FntSetFont(VgaBaseToVgaFont(newfont));
        
    FldSetAttributes(fldptr, &attr);
    FldGetBounds(fldptr, &rect);    
    fieldwidth = DocAppInfo.dprop.fiwikey ? DocAppInfo.dprop.fiwikey*320 : ppscw;
    
    if(rect.extent.x != fieldwidth){
        rect.extent.x = fieldwidth;
        rect.topLeft.x = 0;
        FldSetBounds(fldptr, &rect);
        FldRecalculateField(fldptr, true);
    }

    FldDrawField(fldptr);    
    RefreshMultiControlBitmaps(EDITABLE|LEFTRIGHT|READONLY|BACKERASE);
    scrspeed = FldGetVisibleLines(fldptr);
            
    if(!(QedPrefs.flags1&FSCROLL))
        scrspeed -= 1;
}

//*****************************************************************
// 
// Search
// 
//*****************************************************************

static void Search()
{
Int32 dum, spos;
MemPtr stp;
                    
    Field2Recs(false);
    stp = MemHandleLock(seathand);

    ShowBusy("Searching...");
    spos = srbits & SRF_TOP ? 0 : GetSpIpAbs(&dum, &dum);
   
    if(SearchText(stp, spos, NULL, srbits&(SRF_CASE|SRF_WORD)) != -1){
        srbits &= ~SRF_TOP;
    
        if(srbits & SRF_REPLACE)
            srbits |= SRF_REPFOUND;
    }

    MemHandleUnlock(seathand);
}

//*****************************************************************
// 
// Replace
// 
//*****************************************************************

static void Replace()
{
MemPtr rtp;
Int32 len;

    rtp = MemHandleLock(repthand);
    len = StrLen(rtp);
    
    if(len)
        FldInsert(fldptr, rtp, len);
    else
        FldCut(fldptr);

    MemHandleUnlock(repthand);
}

//*****************************************************************
// 
// ReplaceAll
// 
//*****************************************************************

static void ReplaceAll()
{
Int16 ScreenX, ScreenY;
Boolean PenDown;

    if(compbyte == 0x01)            // suppress dialogs if document is not compressed
        srbits |= SRF_REPALL;
    
    do{
        srbits &= ~SRF_REPFOUND;
        Search();
        
        if(srbits & SRF_REPFOUND)
            Replace();        
		
		EvtGetPen(&ScreenX, &ScreenY, &PenDown);
    }while(srbits&SRF_REPFOUND && !PenDown);
    
    srbits &= ~(SRF_REPFOUND | SRF_REPALL);
}

//*****************************************************************
// 
// SearchText
// 
//*****************************************************************

static Int32 SearchText(Char *text, Int32 apos, Char *bookm, UInt8 mode)
{
Int32 fras, fpos;
MemHandle memhand;
MemPtr memptr, findptr;
UInt16 sr, i, l, l2, sstart;
Int16 j, et, st;
UInt8 slen;
UInt8 *qp, *dp;
 
    // determine record to start search with
    fras = 0;
    sr = STARTREC;

    while(fras<=apos && sr<=lastrec)
        fras += rlen[sr++]; 
    
    fras -= rlen[--sr];
    sstart = apos - fras;

    if(text)
        slen = StrLen(text);
    
    for(i=sr; i<=lastrec; i++){
        l = rlen[i];
        l2 = 0;
        
        memhand = DmNewHandle(qedDB, l+42);    // oversize for overlap + reserve
        ErrFatalDisplayIf(memhand==0, "Low Mem!");
        memptr = MemHandleLock(memhand);      
        Rec2Mem(memptr, i, 1, l);               // offset 1 for word search 'frame'
        
        if(i != lastrec){
            l2 = rlen[i+1];

            if(l2 > 24)
                l2 = 24;

            Rec2Mem(memptr, i+1, l+1, l2);      // offset l+1 for word search 'frame' 
        }

        DmSet(memptr, 0, 1, i==STARTREC ? ' ' : 'x');  // set a 'frame' for word search
        DmSet(memptr, l+l2+1, 1, l2!=24 ? ' ' : 'x');
        
        if(text){
            if((findptr = InStrCl(text, mode, memptr+1, sstart, l+l2, slen))){
                fpos = fras + findptr - memptr-1;     // absolute position

                if(bookm){
                    l = StrLen(text);
                    qp = findptr + l;
                    dp = bookm;
                    dp[0] = '\0';
                                    
                    if(qp[0]>=' ' && qp[0]!='>'){
                        j = 0;

                        while(qp[j]>=' ' && j<15 && fpos+j+l<acdbsize)
                            dp[j] = qp[j++]; 
                    
                        dp[j] = '\0';
                    }
                }
            }
        }
        else{   // text=NULL, search for autobookchar
            qp = memptr+1;
            dp = bookm;
            et = st = 0;
            
            for(j=l+l2; j>l+l2-16; j--){
                if(qp[j] == '>') 
                    et = j - 1;                    
                
                if(et!=0 && qp[j]=='<'){
                    st = j + 1;
                    break;
                } 
            }

            if(et && st)
                for(j=st; j<=et; j++)
                    *dp++ = qp[j];   
                        
            *dp = '\0';
        }
               
        MemPtrFree(memptr);

        if(findptr)
            break;
        
        fras += l;
        sstart = 0;
    }
     
    if(text){
        if(!bookm && !(srbits&SRF_GLOBSEA)){
            ShowBusy(NULL);
            
            if(findptr){
                UpdateField(fpos, fpos+slen, false);
                fpos -= frabs;                  // relative
                // FldScrollField(fldptr, 2, up);
                FldScrollH(MAXFW, false);
                FldSetSelection(fldptr, fpos, fpos+slen);
                FldAutoScroll();
                UpdateScrollBar(true);
                return 0;
            }
            else{
                FrmCustomAlert(alertID_info, "no match", " ", " ");
                return -1;
            }
        }
        else if(bookm || (srbits&SRF_GLOBSEA)){
            if(findptr)
                return fpos;
            else
                return -1;
        }
    }
}

//*****************************************************************
// 
// InStrCl
// 
//*****************************************************************

static MemPtr InStrCl(Char *text, UInt8 mode, MemPtr memp, UInt16 offs, UInt16 size, UInt8 slen)
{
register Char *sp, *mp, *end;
register Char sb, orb;
register UInt8 c;
register Char *mp2, *sp2;

    mp = memp + offs;
    end = memp + size;
    sp = text;
    orb = mode & SRF_CASE ? 0x00 : 0x20;
    sb = *sp++ | orb;

    while(mp < end){
        if((*mp++ | orb) == sb){
            c = 1;
            mp2 = mp;
            sp2 = sp;
            
            while((*mp2 | orb)==(*sp2 | orb) && mp2<end && c<slen){
                mp2++;
                sp2++;
                c++;
            }
            
            if(c == slen){
                if(mode&SRF_WORD){
                    if(!CharValid(*(mp-2)) && !CharValid(*(mp+slen-1)))
                        return --mp;
                }
                else{
                    return --mp;    
                }
            }
        }
    }

    return 0;
}

//*****************************************************************
// 
// GetUserName
// 
//*****************************************************************

static Char* GetUserName(Char *on)
{
#ifdef INSTITUTIONAL
	return(StrCopy(on, "qqqqqqqqqqQQQQQQQQQQqqqqqqqqqqQQ"));
#else       
#define OFFSET 40       
Char buf[OFFSET+dlkUserNameBufSize];
UInt16 size = OFFSET+dlkUserNameBufSize;
    
    if(osversion < 0x05000000){
        buf[OFFSET] = '\0';                                     // if pref does not exist
        PrefGetAppPreferences('psys', 4, buf, &size, false);    // != noPreferenceFound
        buf[OFFSET+dlkUserNameBufSize-1] = '\0';                // secure end
        return(StrCopy(on, buf+OFFSET));
    }
    else{                                   
        if(DlkGetSyncInfo(NULL, NULL, NULL, on, NULL, NULL) != errNone)
            on[0] = '\0';
            
        return(on);
    }
#endif
}

//*****************************************************************
// 
// CRC_Calc
// 
//*****************************************************************

UInt16 CRC_Calc(Char *in)
{
register UInt16 w, out = 0;
register Int16 i, j;
register Boolean rcflag;    

    StrToLower(in, in);
    
    for(i=0; (w=in[i])!='\0'; i++){
        if(w>='a' && w<='z'){
            for(j=0; j<8; j++){
                rcflag = (out & 0x8000) != 0x0000;
                out <<= 1;
                
                if((w & 0x0080) != 0x0000) 
                    out = out | 0x0001;
                
                w = (w & 0x007f) << 1;
                
                if(rcflag) 
                    out ^= 0x1021;
            }
        }
    }
    
    if(!out){               // if 0 use a default value
        out = 0x7c1f;       
        out ^= 0x5ba4;      // result is 10171
    }
    
    return out;
}

//*****************************************************************
// 
// AskAddBookmark
// 
//*****************************************************************

static void AskAddBookmark()
{
MemPtr vp;
Int32 spabs, ipabs;
    
    vp = MemHandleLock(gtinfo->texthand);
     
    if(StrLen(vp)){
        GetSpIpAbs(&spabs, &ipabs);
        AddBookmark(vp, spabs);
    }
    
    MemPtrUnlock(vp);
    FreeOldBookMem();
    FrmReturnToForm(formID_main);
}

//*****************************************************************
// 
// AddBookmark
// 
//*****************************************************************

static void AddBookmark(Char *newbook, Int32 apos)
{
MemHandle rechand;
MemPtr recptr;
UInt32 len;    
UInt16 index;    
    
    len = StrLen(newbook);
    numbook++;
    index = lastrec + numbook;
    
    rechand = DmNewRecord(docDB, &index, 20);
    recptr = MemHandleLock(rechand);

    DmWrite(recptr, 0, newbook, len);
    DmSet(recptr, len, 16-len, 0);
    DmWrite(recptr, 16, &apos, 4);
    
    MemHandleUnlock(rechand);
    DmReleaseRecord(docDB, index, false);
    lbooksel = numbook - 1;
}

//*****************************************************************
// 
// DeleteBookmark
// 
//*****************************************************************

static void DeleteBookmark(UInt16 psel)
{
    DmRemoveRecord(docDB, lastrec+1+psel);
    numbook--;
    PrepareBookList(false);
    LstDrawList(bmlstptr);
}

//*****************************************************************
// 
// GotoBookmark
// 
//*****************************************************************

static void GotoBookmark(UInt16 sel)
{
MemHandle rechand;
MemPtr recptr;
UInt32 p;
UInt16 index;
    
    index = lastrec + 1 + sel;
    rechand = DmGetRecord(docDB, index);
    recptr = MemHandleLock(rechand);
    p = *(UInt32*)(recptr + 16);
    MemHandleUnlock(rechand);
    DmReleaseRecord(docDB, index, false);

    if(p < acdbsize){
        UpdateField(p, -1, false);
        UpdateScrollBar(true);
    }
}

//*****************************************************************
// 
// AskRenameBookmark
// 
//*****************************************************************

static void AskRenameBookmark()
{
MemPtr vp;
    
    vp = MemHandleLock(gtinfo->texthand);
    
    if(StrLen(vp)){
        ChangeBookmark(LstGetSelection(bmlstptr), 0, vp);   // ChangeBookmark(bmlstptr->currentItem, 0, vp);
        PrepareBookList(false);
        LstDrawList(bmlstptr);
    }
    
    MemPtrUnlock(vp);
}

//*****************************************************************
// 
// PrepareBookList
// 
//*****************************************************************

static void PrepareBookList(Boolean heightset)
{
UInt16 i, nb;
UInt32 off=0, len;
MemHandle rechand, bnhand;
MemPtr recptr;

    FreeOldBookMem();
    
    if(numbook){
        bnhand = DmNewHandle(docDB, numbook*16);
        bnptr = MemHandleLock(bnhand);
    
        for(i=lastrec+1; i<lastrec+numbook+1; i++){
            rechand = DmGetRecord(docDB, i);
            recptr = MemHandleLock(rechand);
            len = StrLen(recptr) + 1;
            DmWrite(bnptr, off, recptr, len);                         
            off += len;
            MemHandleUnlock(rechand);
            DmReleaseRecord(docDB, i, false);
        }
        
        bpaptr = (Char**)MemHandleLock(SysFormPointerArrayToStrings(bnptr, numbook));
        LstSetListChoices(bmlstptr, bpaptr, numbook);
        if(lbooksel >= numbook) lbooksel = numbook-1;
        LstSetTopItem(bmlstptr, lbooksel-4<0 ? 0 : lbooksel-4);
        LstSetSelection(bmlstptr, lbooksel);
    }
    else{
        LstSetListChoices(bmlstptr, NULL, NULL);  
    }
    
    if(heightset){
        nb = (ppsch-BARHIGHT-14-GSIEXT) / stdfh;    // maximum for screenhight   
        if(nb > MAXBOOK) nb = MAXBOOK;
        if(numbook < nb) nb = numbook;
        if(nb < 3) nb = 3;
        LstSetHeight(bmlstptr, nb);
    }
}

//*****************************************************************
// 
// AutoScanBookmarks
// 
//*****************************************************************

static void AutoScanBookmarks(void)
{
Char bmstring[17];

    if(!numbook && dbsize>16 && QedPrefs.flags1&AUTOSCAN && !(dbAttr&dmHdrAttrReadOnly)){
        SearchText(NULL, dbsize-16, bmstring, SRF_CASE);
        bmstring[12] = '\0';    // 12 chars max.
        
        if(StrLen(bmstring))
            ScanBookmarks(bmstring);
    }
}

//*****************************************************************
// 
// ScanBookmarks
// 
//*****************************************************************

static void ScanBookmarks(Char *text)
{
Char bookm[16], bm[16];
Int32 apos=0, bpos, len=0;
UInt16 i;
Boolean bmflag;
MemHandle rechand;
MemPtr recptr;

    Field2Recs(false);
    ShowBusy("Scanning bookmarks...");

    while((apos = SearchText(text, apos+len, bookm, SRF_CASE)) != -1){
        len = StrLen(bookm) + 1; 
        bmflag = true;
                
        if(len > 1){        
            for(i=0; i<numbook; i++){
                rechand = DmQueryRecord(docDB, lastrec+1+i);
                recptr = MemHandleLock(rechand);
                StrNCopy(bm, recptr, 16);
                bpos = *((Int32*)(recptr+16));
                MemHandleUnlock(rechand);
                
                if(!StrCompare(bm, bookm)){     // is bookmark already known?
                    bmflag = false;             // yes, do not add bookmark
                        
                    if(bpos != apos)            // have we to change it?
                        ChangeBookmark(i, apos, NULL);
                    
                    break;
                }
            }
            
            if(bmflag)
                AddBookmark(bookm, apos);
        }
    }

    lbooksel = 0;
    FreeOldBookMem();
    ShowBusy(NULL);
}

//*****************************************************************
// 
// ChangeBookmark
// 
//*****************************************************************

static void ChangeBookmark(UInt16 i, Int32 apos, Char *name)
{
MemHandle rechand;
MemPtr recptr;
UInt16 index;
UInt32 len;    

    index = lastrec + 1 + i;
    rechand = DmGetRecord(docDB, index);
    recptr = MemHandleLock(rechand);
    
    if(name == NULL){
        DmWrite(recptr, 16, &apos, 4);
    }
    else{
        len = StrLen(name);
        DmWrite(recptr, 0, name, len);
        DmSet(recptr, len, 16-len, 0);
    }
    
    MemHandleUnlock(rechand);
    DmReleaseRecord(docDB, index, false);
}

//*****************************************************************
// 
// SortBookmarks
// 
//*****************************************************************

static void SortBookmarks(Int16 sel, Int16 sortkind)
{
Int16 index, i;
MemHandle rechand1, rechand2;
MemPtr recptr1, recptr2;
Boolean sflag, cflag;

    index = lastrec + 1 + sel;

    if(sortkind == 0){                   // up
        if(sel > 0){
            DmMoveRecord(docDB, index, index-1);
            lbooksel--;
        }
    }
    else if(sortkind == 1){             // down
        if(sel < numbook-1){
            DmMoveRecord(docDB, index, index+2);
            lbooksel++;
        }
    }
    else{                               // alpha or position sort
        do{
            sflag = false;
            
            for(i=lastrec+1; i<lastrec+numbook; i++){
                cflag = false;
                rechand1 = DmQueryRecord(docDB, i);
                recptr1 = MemHandleLock(rechand1);
                rechand2 = DmQueryRecord(docDB, i+1);
                recptr2 = MemHandleLock(rechand2);

                if(sortkind == 2){      // alpha sort
                    if(StrNCompare(recptr1, recptr2, 16) > 0)
                        cflag = true;
                }
                else{                   // position sort (3)
                    if(*((UInt32*)(recptr1+16)) > *((UInt32*)(recptr2+16)))
                        cflag = true;
                }

                MemHandleUnlock(rechand2);
                MemHandleUnlock(rechand1);

                if(cflag){
                    DmMoveRecord(docDB, i, i+2);
                    sflag = true;
                }
            }
        }while(sflag);
    }

    PrepareBookList(false);
    LstDrawList(bmlstptr);
}

//*****************************************************************
// 
// FreeOldBookMem
// 
//*****************************************************************

static void FreeOldBookMem()
{
    if(bnptr)
        MemPtrFree((MemPtr)bnptr);

    if(bpaptr)
        MemPtrFree((MemPtr)bpaptr);

    bnptr = 0;
    bpaptr = 0;
}

//*****************************************************************
// 
// GetSpIpAbs
// 
//*****************************************************************

static Int32 GetSpIpAbs(Int32 *spabs, Int32 *ipabs)
{
Int32 apos;

    *spabs = apos = frabs + FldGetScrollPosition(fldptr);
    
    if(QfSetGetAttr(GETIPVIS))
        *ipabs = apos = frabs + FldGetInsPtPosition(fldptr); 
    else
        *ipabs = -1;

    return apos;
}

//****************************************************************
// 
// SetCategoryFilter
// 
//*****************************************************************

static void SetCategoryFilter(UInt16 sel)
{
    QedPrefs.category = CategoryFind(qedDB, LstGetSelectionText(catlstptr, sel));
    RefreshOpenDialog();
}

//****************************************************************
// 
// PopupDocCatList
// 
//*****************************************************************

static void PopupDocCatList(Char *seltext, Int16 y)
{
ListPtr dclptr;
LocalID dbid;
UInt8 dcat=0, newcat, att;
Int16 newsel;
UInt16 csel;
Boolean chflag=false;
Char name[16];

    dbid = DmFindDatabase(0, seltext);
    att = DocAttribute(dbid, NULL);
    dcat =  att & dmRecAttrCategoryMask;

    CategoryGetName(qedDB, dcat, name);
    
    if(!StrLen(name)) 
        dcat = UNFILED;   
    
    dclptr = GetObjectPointer(listID_doccat);
    CategoryCreateList(qedDB, dclptr, dcat, false, true, NULL, NULL, true);
    LstSetHeight(dclptr, 3);    
    csel = LstGetSelection(dclptr);
    LstSetTopItem (dclptr, csel>0 ? csel-1 : 0);    
    LstSetPosition(dclptr, 40, y);
    
    if((newsel=LstPopupList(dclptr)) != -1){
        newcat = CategoryFind(qedDB, LstGetSelectionText(dclptr, newsel));
        
        if(newcat != dcat){
            att = att & ~dmRecAttrCategoryMask | newcat;
            DocAttribute(dbid, &att);
            chflag = true;
        }
    }

    CategoryFreeList(qedDB, dclptr, false, NULL);
    
    if(chflag)
        RefreshOpenDialog();
}

//****************************************************************
// 
// GetObjectPointer
// 
//*****************************************************************

static MemPtr GetObjectPointer(UInt16 obid)
{
FormPtr form;

    form = FrmGetActiveForm();
    return FrmGetObjectPtr(form, FrmGetObjectIndex(form, obid));
}

//*****************************************************************
//
// SetObjectPosition
//
//*****************************************************************

static void SetObjectPosition(UInt16 obid, Int16 x, Int16 y)
{
FormPtr form;

    form = FrmGetActiveForm();
    FrmSetObjectPosition(form, FrmGetObjectIndex(form, obid), x, y);
}

//*****************************************************************
//
// ShowObject
//
//*****************************************************************

static void ShowObject(UInt16 obid)
{
FormPtr form;

    form = FrmGetActiveForm();
    FrmShowObject(form, FrmGetObjectIndex(form, obid));
}

//*****************************************************************
//
// ShowObject
//
//*****************************************************************

static void HideObject(UInt16 obid)
{
FormPtr form;

    form = FrmGetActiveForm();
    FrmHideObject(form, FrmGetObjectIndex(form, obid));
}

//****************************************************************
// 
// RefreshOpenDialog
// 
//*****************************************************************

static void RefreshOpenDialog()
{
    CategoryFreeList(qedDB, catlstptr, true, NULL);
    GetDBnames();
    PrepOpenDialog(false);
    LstDrawList(dblstptr);    
}

//****************************************************************
// 
// AddCategory
// 
//*****************************************************************

static void AddCategory()
{
LocalID apinid;
QedCatAppInfoPtr apinptr;
MemPtr vp;
UInt16 i, delcat;    
    
    apinid = DmGetAppInfoID(qedDB);
    apinptr = MemLocalIDToLockedPtr(apinid, 0);
    vp = MemHandleLock(gtinfo->texthand);

    for(i=1; i<dmRecNumCategories-1; i++){
        if(*(apinptr->categoryLabels[i]) == '\0'){
            DmStrCopy(apinptr, i*dmCategoryLength+sizeof(UInt16), vp);
            delcat = apinptr->renamedCategories & ((1<<i)^0xffff);
            DmWrite(apinptr, 0, &delcat, 2);
            break;
        }
    }

    MemPtrUnlock(apinptr);
    MemPtrUnlock(vp);
    RefreshOpenDialog();
}

//****************************************************************
// 
// DeleteCategory
// 
//*****************************************************************

static void DeleteCategory()
{
UInt8 cat;
MemPtr vp;
LocalID apinid;
QedCatAppInfoPtr apinptr;
UInt16 delcat;
    
    vp = MemHandleLock(gtinfo->texthand);
    cat = CategoryFind(qedDB, vp);
    MemPtrUnlock(vp);
    
    if(cat!=UNFILED && cat<dmRecNumCategories){
        apinid = DmGetAppInfoID(qedDB);
        apinptr = MemLocalIDToLockedPtr(apinid, 0);
        DmSet(apinptr, cat*dmCategoryLength+sizeof(UInt16),1 ,0);      
        delcat = apinptr->renamedCategories | (1<<cat);
        DmWrite(apinptr, 0, &delcat, 2);
        MemPtrUnlock(apinptr);
        QedPrefs.category = 255;    // all
        RefreshOpenDialog();
    }
}

//****************************************************************
// 
// RenameCategory
// 
//*****************************************************************

static void RenameCategory()
{
UInt8 cat;
MemPtr vp;
LocalID apinid;
QedCatAppInfoPtr apinptr;
    
    cat = QedPrefs.category;
    
    if(cat!=UNFILED && cat<dmRecNumCategories){
        apinid = DmGetAppInfoID(qedDB);
        apinptr = MemLocalIDToLockedPtr(apinid, 0);
        vp = MemHandleLock(gtinfo->texthand);
        if(StrLen(vp))
            DmStrCopy(apinptr, cat*dmCategoryLength+sizeof(UInt16), vp);      
        MemPtrUnlock(vp);
        MemPtrUnlock(apinptr);
        RefreshOpenDialog();
    }
}

//****************************************************************
// 
// PopupGetText
// 
//*****************************************************************

static void PopupGetText(Char *title, Char *text, Int16 x, Int16 y, Int16 c, MemPtr fp, Char *checktext1)
{
MemHandle vh;
MemPtr vp;
    
    gtinfo = MemPtrNew(sizeof(gtinfotype));
    
    vh = gtinfo->texthand = DmNewHandle(qedDB, 33);
    vp = MemHandleLock(vh);
    DmStrCopy(vp, 0, text);
    MemHandleUnlock(vh);
    
    StrCopy(gtinfo->title, title);
    StrCopy(gtinfo->checktext1, checktext1);
    gtinfo->x = x;
    gtinfo->y = y;
    gtinfo->cwidth = c;
    gtinfo->id = FrmGetActiveFormID();
    gtinfo->funcptr = fp;
        
    FrmPopupForm(formID_gettext);
}

//****************************************************************
// 
// PrepPrefsForm
// 
//*****************************************************************

static void PrepPrefsForm(void)
{
dproperties *dpptr;
ListPtr lstptr;
FormPtr form;
ControlPtr cp;

    StrCopy(ptemp->name, actdbname);
    
    if(osversion >= 0x03000000l)
        CtlSetUsable(GetObjectPointer(pushbuttonID_lboldfont), true);
    
    if(ptemp->menuid == menuitemID_qedpref){
        dpptr = &QedPrefs.dprop;
        FldSetTextPtr(GetObjectPointer(fieldID_filename), "Default settings");
        CtlSetUsable(GetObjectPointer(checkID_fscroll), true);
        CtlSetUsable(GetObjectPointer(checkID_hardbuttons), true);
        CtlSetUsable(GetObjectPointer(checkID_autoindent), true);    
        CtlSetUsable(GetObjectPointer(checkID_autoscan), true);
        CtlSetUsable(GetObjectPointer(checkID_doubletap), true);
        
        if(qflags&COLORDEVICE){
            CtlSetUsable(GetObjectPointer(checkID_monochrome), true);
            CtlSetV(checkID_monochrome, QedPrefs.flags1&MONOCHROME);
        }
        
        ShowObject(labelID_tab);
        CtlSetUsable(GetObjectPointer(popuptriggerID_tab), true);
        lstptr = GetObjectPointer(listID_tab);
        LstSetSelection(lstptr, QedPrefs.tabwidth);
        CtlSetLabel(GetObjectPointer(popuptriggerID_tab), LstGetCurrentSelText(lstptr));
        
        CtlSetV(checkID_fscroll, QedPrefs.flags1&FSCROLL);
        CtlSetV(checkID_hardbuttons, QedPrefs.flags1&HARDBUTTONS);
        CtlSetV(checkID_autoindent, QedPrefs.flags1&AUTOINDENT);
        CtlSetV(checkID_autoscan, QedPrefs.flags1&AUTOSCAN);
        CtlSetV(checkID_doubletap, QedPrefs.flags1&DOUBLETAP);
    }
    else if(ptemp->menuid == menuitemID_docpref){
        DocAppInfo.dprop.editable = editable;
        dpptr = &DocAppInfo.dprop;
        FldSetTextPtr(GetObjectPointer(fieldID_filename), ptemp->name);
        CtlSetUsable(GetObjectPointer(checkID_backup), true);
        CtlSetV(checkID_backup, dbAttr&dmHdrAttrBackup);
        
        if(qflags&READONLYCHANGE){
            CtlSetUsable(GetObjectPointer(checkID_readonly), true);
            CtlSetV(checkID_readonly, dbAttr&dmHdrAttrReadOnly);
        }
        
        CtlSetUsable(GetObjectPointer(checkID_private), true);
        CtlSetV(checkID_private, DocAttribute(actdbID, NULL)&dmRecAttrSecret);
    }

    lstptr = GetObjectPointer(listID_fieldwidth);
    LstSetSelection(lstptr, dpptr->fiwikey);
    CtlSetLabel(GetObjectPointer(popuptriggerID_fieldwidth), LstGetCurrentSelText(lstptr));
    
    form = FrmGetActiveForm();

    if(repfont[0].sysptr){
        cp = FrmGetObjectPtr(form, FrmGetObjectIndex(form, pushbuttonID_repfont1));
        cp->font = REPFONT1;
        *(cp->text) = 'A';
    }

    if(repfont[1].sysptr){
        cp = FrmGetObjectPtr(form, FrmGetObjectIndex(form, pushbuttonID_repfont2));
        cp->font = REPFONT2;
        *(cp->text) = 'A';
    }    

    FrmSetControlGroupSelection(form, groupID_fontgroup, dpptr->font+FONTBUTTONIDSTART);

    CtlSetV(checkID_editable, dpptr->editable);
    CtlSetV(checkID_ruled, dpptr->ruled);
}

//****************************************************************
// 
// SavePrefs
// 
//*****************************************************************

static void SavePrefs(FormPtr form)
{
dproperties *dpptr;
Boolean renflag=false, npriv=false;
UInt16 index;
UInt8 att;

    if(ptemp->menuid == menuitemID_qedpref){
        dpptr = &QedPrefs.dprop;
        QedPrefs.flags1 = CtlGetV(checkID_fscroll) ? QedPrefs.flags1|FSCROLL : QedPrefs.flags1&~FSCROLL;
        QedPrefs.flags1 = CtlGetV(checkID_hardbuttons) ? QedPrefs.flags1|HARDBUTTONS : QedPrefs.flags1&~HARDBUTTONS;
        QedPrefs.flags1 = CtlGetV(checkID_autoindent) ? QedPrefs.flags1|AUTOINDENT : QedPrefs.flags1&~AUTOINDENT;
        QedPrefs.flags1 = CtlGetV(checkID_autoscan) ? QedPrefs.flags1|AUTOSCAN : QedPrefs.flags1&~AUTOSCAN;
        QedPrefs.flags1 = CtlGetV(checkID_doubletap) ? QedPrefs.flags1|DOUBLETAP : QedPrefs.flags1&~DOUBLETAP;
        QedPrefs.tabwidth = LstGetSelection(GetObjectPointer(listID_tab));

        if(qflags&COLORDEVICE)
            QedPrefs.flags1 = CtlGetV(checkID_monochrome) ? QedPrefs.flags1|MONOCHROME : QedPrefs.flags1&~MONOCHROME;

    }
    else if(ptemp->menuid == menuitemID_docpref){
        dpptr = &DocAppInfo.dprop;
        renflag = StrCompare(actdbname, ptemp->name);
        att = DocAttribute(actdbID, NULL);
        npriv = CtlGetV(checkID_private);
        
        if(((att&dmRecAttrSecret)!=0)!=npriv && !(dbAttr&dmHdrAttrReadOnly)){
            att = att&~dmRecAttrSecret | npriv ? dmRecAttrSecret : 0x00;
            DocAttribute(actdbID, &att);         
        }

        dbAttr = CtlGetV(checkID_backup) ? dbAttr|dmHdrAttrBackup : dbAttr&(~dmHdrAttrBackup);
        
        if(qflags&READONLYCHANGE)
            dbAttr = CtlGetV(checkID_readonly) ? dbAttr|dmHdrAttrReadOnly : dbAttr&(~dmHdrAttrReadOnly);
    }

    dpptr->fiwikey = LstGetSelection(GetObjectPointer(listID_fieldwidth));
    
    index = FrmGetControlGroupSelection(form, groupID_fontgroup);
    dpptr->font = FrmGetObjectId(form, index) - FONTBUTTONIDSTART;
    dpptr->editable = CtlGetV(checkID_editable);
    dpptr->ruled = CtlGetV(checkID_ruled);

    if(ptemp->menuid == menuitemID_qedpref)
        MemMove(&DocDefaultAppInfo.dprop, dpptr, sizeof(dproperties));

    if(renflag)
        RenameFile(actdbname, ptemp->name);

    if(npriv || renflag)
        GetDBnames();
}

//****************************************************************
// 
// CtlSetV
// 
//*****************************************************************

static void CtlSetV(UInt16 id, Boolean v)
{
    CtlSetValue(GetObjectPointer(id), v);
}

//****************************************************************
// 
// CtlGetV
// 
//*****************************************************************

static Boolean CtlGetV(UInt16 id)
{
    return CtlGetValue(GetObjectPointer(id));
}

//****************************************************************
// 
// ChangePrefsDBN
// 
//*****************************************************************

static void ChangePrefsDBN()
{
MemPtr vp;

    vp = MemHandleLock(gtinfo->texthand);        

    if(StrLen(vp))
        ChangePrefsPar(fieldID_filename, ptemp->name, vp);

    MemPtrUnlock(vp);
}

//****************************************************************
// 
// ChangePrefsPar
// 
//*****************************************************************

static void ChangePrefsPar(UInt16 id, Char *toptr, Char *fromptr)
{
FieldPtr fp;

    fp = GetObjectPointer(id);
    StrCopy(toptr, fromptr);
    FldSetTextPtr(fp, toptr);
    FldDrawField(fp);    
}

//****************************************************************
// 
// FetchDocAppInfo
// 
//*****************************************************************

static void FetchDocAppInfo(LocalID dbid, MemPtr vp)
{
DocAppInfoPtr apinptr, daip;

    daip = (DocAppInfoPtr)vp;
    
    apinptr = HandleDocAppInfo(dbid);
    MemMove(daip, apinptr, sizeof(DocAppInfo));
    MemPtrUnlock(apinptr);              // close as soon as possible    

    if(DocAppInfo.dprop.fiwikey > 4)    // number of entries in listID_fieldwidth
        DocAppInfo.dprop.fiwikey = 0;
}

//****************************************************************
// 
// UpdateDocAppInfo
// 
//*****************************************************************

static void UpdateDocAppInfo(LocalID dbid, MemPtr vp)
{
DocAppInfoPtr apinptr, daip;

    daip = (DocAppInfoPtr)vp;
    apinptr = HandleDocAppInfo(dbid);
    DmWrite(apinptr, 0, daip, sizeof(DocAppInfo));
    MemPtrUnlock(apinptr);          // close as soon as possible
}

//*****************************************************************
//
// ReplaceFont
//
//*****************************************************************

static void ReplaceFont(Char* name, UInt16 fid, UInt8 n)
{
LocalID dbid;
DmOpenRef openref;
MemHandle memhandle;
MemPtr ptr;
UInt16 i;    
    
    if(dbid=DmFindDatabase(0, name)){
        if(openref=DmOpenDatabase(0, dbid, dmModeReadOnly)){
            for(i=0; i<NUMRESINDB; i++){
                if(memhandle=DmQueryRecord(openref, i)){
                    if(ptr=MemHandleLock(memhandle)){
                        repfont[n].id = fid;
                        repfont[n].sysptr = SetCustomFont(fid, ptr);
                    }
                }
    
                n += 2;         // for Sony hires fonts
                fid += 8;
            }

            DmCloseDatabase(openref);
        }
    }
}

//****************************************************************
// 
// SetCustomFont
// 
//*****************************************************************

static MemPtr SetCustomFont(UInt16 index, MemPtr ptr)
{
UInt32 oldptr;
UInt32 *p;

	if(!ptr)
		return 0;

	//if(index>7 || (osversion<0x03000000l&&index>6))
	//  index=0;

	if(osversion < 0x03000000l)
	    p = ((UInt32 *)0x1d2) + index;
	else
		p = (*((UInt32 **)0x1da)) + index;

	oldptr = *p;
	*p = (UInt32)ptr;
	
	return (MemPtr)oldptr;
}

//****************************************************************
// 
// HandleDoubleTap
// 
//*****************************************************************

static void HandleDoubleTap(void)
{
UInt16 ip;
Char *tp, *z1, *z2;

    ip = FldGetInsPtPosition(fldptr);
    tp = FldGetTextPtr(fldptr);
    z1 = z2 = tp + ip;
    
    if(CharValid(*z1) && CharValid(*(z1-1))){
        while(CharValid(*z1) && z1>=tp)
            z1--;    
           
        while(CharValid(*z2))
            z2++;
        
        FldSetSelection(fldptr, z1-tp+1, z2-tp);
        FldCopy(fldptr);
    }            
    else if(!(dbAttr&dmHdrAttrReadOnly)){
        FldPaste(fldptr);
        CheckField();
    }        
}

//****************************************************************
// 
// CharValid
// 
//*****************************************************************

static Boolean CharValid(UInt8 c)
{
    return c>='a' || (c>='@' && c<='Z') || (c>='0' && c<='9');
}

#ifdef NORMPALM
//****************************************************************
// 
// SimTapCount
// 
//*****************************************************************

static void SimTapCount(EventPtr event)
{
static UInt32 tlast;
UInt32 ti;

    ti = TimGetTicks();
    
    if(ti-tlast <= 40)
        event->tapCount++;
    else
        event->tapCount = 1;

    tlast = ti;
}             
#endif

//****************************************************************
// 
// SwitchBar
// 
//*****************************************************************

static void SwitchBar(Boolean restore)
{
UInt16 i, ip, offs, rfh;
RectangleType rect;
Boolean baron, recnec=false;
IndexedColorType prevcol;

    baron = QedPrefs.baron;
    ip = FldGetInsPtPosition(fldptr);
    FldReleaseFocus(fldptr);            // FldSetBounds needs the insertation point off
    FldGetBounds(fldptr, &rect);

    if(rect.extent.x != fieldwidth)
        recnec = true;
        
    rect.extent.x = fieldwidth;
    rect.extent.y = (baron ? ppsch-BARHIGHT : ppsch) - 1;
    FldSetBounds(fldptr, &rect);
    
    if(recnec)
        FldRecalculateField(fldptr, true);

    scrspeed = FldGetVisibleLines(fldptr);
    
    if(QfSetGetAttr(GETIPVIS) && restore){
        FldSetInsPtPosition(fldptr, ip);
        FldGrabFocus(fldptr);
    }    
    
    rect.topLeft.x = 0;
    rect.extent.x = ppscw;
    
    if((rfh=scrspeed*FntLineHeight()) != ppscw-BARHIGHT){
        rect.topLeft.y = rfh;
        rect.extent.y = ppsch-rfh-BARHIGHT;                      
        WinEraseRectangle(&rect, 0);
    }

    rect.topLeft.y = ppsch-BARHIGHT;
    rect.extent.y = BARHIGHT;                      

    if(baron && qflags&COLORMODE)
        prevcol = WinSetBackColor(barbackcol);

    WinEraseRectangle(&rect, 0);

    if(baron && qflags&COLORMODE)
        WinSetBackColor(prevcol);

    if(baron){        
        for(i=0; i<MCBUTNUM; i++)                   // activate controls
            CtlShowControl(GetObjectPointer(FIRSTMCBID+i));
        
        for(i=0; i<MCSCBUTNUM; i++)                 // draw single control bitmaps
            DrawBarBitmap(i, 0, false);             // background is already ready, no backerase
            
        RefreshMultiControlBitmaps(REFRESHALL);     // draw multi control bitmaps, no backerase
    }    
    else{
        for(i=0; i<MCBUTNUM; i++)
            HideObject(FIRSTMCBID+i);
    }

    if(!(QedPrefs.flags1&FSCROLL))
        scrspeed -= 1;    
    
    FldDrawField(fldptr);

    if(baron && restore){
        sbposold = 0;               // force redraw of scroll bar
        UpdateScrollBar(false);
    }
}    

//****************************************************************
// 
// DrawBar
// 
//*****************************************************************

static void DrawBar(void)
{
Boolean bar;
    
    bar = QedPrefs.baron;       // save bar status
    QedPrefs.baron = true;      // show bar even if bar is off
    SwitchBar(false);           // show bar
    DrawScrollBar(SCROLLBARLEFT, MINSBCARSIZE);
    QedPrefs.baron = bar;       // restore bar status
}

//****************************************************************
// 
// DocAttribute
// 
//*****************************************************************

static UInt8 DocAttribute(LocalID dbid, UInt8 *value)
{
UInt16 attr=0;
DmOpenRef dbP;

    if(dbid != actdbID)
        dbP = DmOpenDatabase(0, dbid, dmModeReadWrite);
    else
        dbP = docDB;

    if(value){
        attr = *value;
        DmSetRecordInfo(dbP, 0, &attr, NULL);
    }

    DmRecordInfo(dbP, 0, &attr, NULL, NULL);    

    if(dbid != actdbID)
        DmCloseDatabase(dbP);
    
    return (UInt8)attr;
}

/*
static UInt8 DocAttribute(LocalID dbid, UInt8 *value)
{
DatabaseHdrPtr dbhptr;
UInt8 att=0;

    dbhptr = MemLocalIDToLockedPtr(dbid, 0); 

    if(dbhptr->recordList.numRecords > 0){
        if(value)
            DmSet(dbhptr, (UInt32)(&dbhptr->recordList.attrib)-(UInt32)dbhptr, 1, *value);       
        
        att = dbhptr->recordList.attrib;
    }
    
    MemPtrUnlock(dbhptr);
    return att;
}
*/

//****************************************************************
// 
// GlobalFindFind
// 
//*****************************************************************

static void GlobalFindFind(FindParamsPtr fip)
{
LocalID dbid;
Char title[22], name[16];
UInt16 i;
RectangleType r;
UInt32 ds, numrec;
UInt8 firstbyte;
Int32 fpos;
DmOpenRef act_docDB;    
UInt8 act_compbyte;
UInt16 act_lastrec;
UInt16 *act_rlen;
UInt32 act_dbsize, act_acdbsize;
Int16 pScreenX, pScreenY;
Boolean pPenDown, found=false;

    Field2Recs(true);                           // update records if necessary

    if(!naptr)                                  // if open dialog wasn't used yet
        GetDBnames();

    act_docDB = docDB;                          // save global variables of actual loaded file
    act_compbyte = compbyte;                        
    act_lastrec = lastrec;                      
    act_rlen = rlen;                                
    act_dbsize = dbsize;
    act_acdbsize = acdbsize;

    srbits |= SRF_GLOBSEA;                      // suppress infotext dialogs during global find

    CategoryGetName(qedDB, QedPrefs.category, name);
    StrCopy(title, "QED (");
    StrCat(title, name);
    StrCat(title, ")");

    if(FindDrawHeader(fip, title))              // show title and category
        return;
    
    for(i=fip->recordNum; i<numdb; i++){        // recordNum is used as dbname index
        FindGetLineBounds(fip, &r);             // show searched filename   
        WinDrawChars(npaptr[i], StrLen(npaptr[i]), r.topLeft.x, r.topLeft.y);

        dbid = DmFindDatabase(0, npaptr[i]);    // get dbid

        if(dbid != actdbID){                    // if db is already loaded
            if(!(docDB=DmOpenDatabase(0, dbid, dmModeReadOnly)))    // if database cannot be opened
                continue;

            firstbyte = GetDocInfo();           // get firstbyte, compbyte and lastrec
            DmDatabaseSize(0, dbid, &numrec, NULL, &ds);
        
            if(numrec<=lastrec || numrec<2 || compbyte<1 || compbyte>2 || ds<16 || RecSize(0)<16){
                DmCloseDatabase(docDB);
                continue;
            }
        
            GetRecLens();                       // get rlen array, dbsize, acdbsize
        }
        else{
            acdbsize = act_acdbsize;            // restore global variables of actual loaded file
            dbsize = act_dbsize;
            rlen = act_rlen;                            
            lastrec = act_lastrec;                      
            compbyte = act_compbyte;                        
            docDB = act_docDB;                          
        }

        fpos = SearchText(fip->strAsTyped, 0, NULL, srbits&(SRF_CASE|SRF_WORD));

        if(dbid != actdbID){                    // close db and free rlen if not actual db
            DmCloseDatabase(docDB);
            MemPtrFree((MemPtr)rlen);           // free rlen array
            MemHeapCompact(0);
        }
                
        if(fpos != -1){
            if(FindSaveMatch(fip, 0, 0, 0, fpos, 0, dbid) || r.topLeft.y>108){
                fip->recordNum = i+1;           // start with i+1 next time
                fip->more = true;               // continue search 
                break;
            }           
            fip->lineNumber++;                  // match, next line for next filename
            found = true;
        }
        else{
            WinEraseRectangle(&r, 0);           // no match, delete filename from form
        }
    
        EvtGetPen(&pScreenX, &pScreenY, &pPenDown);
        
        if(pPenDown){
            fip->recordNum = i+1;           // start with i+1 next time
            fip->more = true;               // do not continue search 
            break;
        }
    }

    if(found){                              // copy search string for internal search
        DmStrCopy(MemHandleLock(seathand), 0, fip->strAsTyped);
        MemHandleUnlock(seathand);
    }

    srbits &= ~SRF_GLOBSEA;                 // enable infotext dialogs

    acdbsize = act_acdbsize;                // restore global variables of actual loaded file
    dbsize = act_dbsize;
    rlen = act_rlen;                            
    lastrec = act_lastrec;                      
    compbyte = act_compbyte;                        
    docDB = act_docDB;                          
}

//****************************************************************
// 
// GlobalFindGoTo
// 
//*****************************************************************

static void GlobalFindGoTo(GoToParamsPtr gtp)
{
    CloseDB(SAVE_FIELD|SAVE_SPIP|AUTOBUSYOFF);
    OpenDBid(gtp->dbID, gtp->searchStrLen, gtp->matchCustom);
}

//****************************************************************
// 
// SaveAs
// 
//*****************************************************************

static void SaveAs()
{
MemPtr vp;
Char newname[dmDBNameLength];
DmOpenRef oref;
MemHandle s_rechand, d_rechand;
MemPtr s_recptr, d_recptr;
UInt8 *zptr;
UInt16 i, index;
LocalID dbid;
Boolean sourcefc, destfc;
UInt32 numrec, size, free, max;
MemPtr mptr;
UInt16 sizew; 
UInt16 hid, hflag;
UInt32 numh, sumfree;
UInt16 backupbit;
DocAppInfoPtr apinptr;

    vp = MemHandleLock(gtinfo->texthand);
    StrCopy(newname, vp);
    MemPtrUnlock(vp);
        
    dbid = DmFindDatabase(0, newname);
    
    if(dbid == actdbID){                    // if actual loaded file                
        FrmCustomAlert(alertID_info, "'", newname, "'\nactually loaded. Choose a different name.");
        return;
    }
    
    if(dbid){                               // if file already exists   
        if(FrmCustomAlert(alertID_okcancel, "'", newname, "'\nalready exists!\nOverwrite it?")){
            return;                         // do not overwrite 
        }
        else{
            if(DmDeleteDatabase(0, dbid)){  // delete file
                FrmCustomAlert(alertID_info, "Can't", " overwrite", " file!");  
                return;         
            }
        }
    }
    
    sourcefc = compbyte==0x02;              // get compress flags
    destfc = gtinfo->check1;
    
    numh = MemNumHeaps(0);                  // get number of heaps
    sumfree = 0;
    
    for(i=0; i<numh; i++){                  // get free bytes of non volatile RAM heaps
        hid = MemHeapID(0, i);

        if(!MemHeapDynamic(hid) && !(MemHeapFlags(hid) & 0x01)){
            MemHeapFreeBytes(hid, &free, &max); 
            sumfree += free;
        }
    }

    if(sumfree < dbsize + 4096 - (destfc ? dbsize/3 : 0)){
        FrmCustomAlert(alertID_info, "Low Mem!\n", "Cannot save", " file.");
        return;
    }
    
    // heapflags = MemHeapFlags(heapid);    // get heapflags
    //if(heapflags & memHeapFlagReadOnly)
    //  ;
    
    DmCreateDatabase(0, newname, docAppID, docDBType, false);
    dbid = DmFindDatabase(0, newname);
    
    if(dbid){       
        ShowBusy("Save as...");
        
        DmDatabaseSize(0, actdbID, &numrec, NULL, NULL);    // get numrec of source DB

        oref = DmOpenDatabase(0, dbid, dmModeReadWrite);
        WriteDocInfo(oref, dbid, dbsize, lastrec, destfc ? 0x02 : 0x01);    // record 0 

        for(i=1; i<numrec; i++){                // text records, 1...lastrec
            index = i;
            
            s_rechand = DmGetRecord(docDB, i);
            s_recptr = MemHandleLock(s_rechand);

            if(sourcefc==destfc || i>lastrec){  // simply copy records + bookmark records
                size = MemHandleSize(s_rechand);
                d_rechand = DmNewRecord(oref, &index, size);
                d_recptr = MemHandleLock(d_rechand);
                DmWrite(d_recptr, 0, s_recptr, size);
            }
            else if(sourcefc && !destfc){       // decompress source
                size = rlen[i];
                d_rechand = DmNewRecord(oref, &index, size);
                d_recptr= MemHandleLock(d_rechand);
                zptr = MemPtrNew(size+12);      // oversize
                DeCompress(zptr, s_recptr, size);
                DmWrite(d_recptr, 0, zptr, size);
                MemPtrFree(zptr);
            }
            else{                               // compress source
                if(i == 1){
                    cptrig = dbsize;
                    InitCompProgress();
                }
                sizew = rlen[i];
                mptr = s_recptr;    
                Compress(&mptr, &sizew);
                d_rechand=DmNewRecord(oref, &index, sizew);
                d_recptr = MemHandleLock(d_rechand);
                DmWrite(d_recptr, 0, mptr, sizew);
                MemPtrFree(mptr);
            }
            
            MemPtrUnlock(d_recptr); 
            DmReleaseRecord(oref, index, false);
            MemPtrUnlock(s_recptr);
            DmReleaseRecord(docDB, i, false);
        }
        
        DmCloseDatabase(oref);
        
        // copy old docappinfo to new file 
        apinptr = HandleDocAppInfo(dbid);
        DmWrite(apinptr, 0, &DocAppInfo, sizeof(DocAppInfo));
        MemPtrUnlock(apinptr);
        
        // copy backup attribute        
        backupbit = dbAttr & dmHdrAttrBackup;
                
        GetDBnames();                       // for next open dialog
        ShowBusy(NULL);
    
        CloseDB(SAVE_SPIP);                 // false because old file already saved
        OpenDBid(dbid, 0, 0);
        dbAttr = backupbit ? dbAttr|dmHdrAttrBackup : dbAttr&(~dmHdrAttrBackup);
    }
}

//****************************************************************
// 
// InitCompProgress
// 
//*****************************************************************

static void InitCompProgress()
{
RectangleType r;
// UInt32 t, d; 
    
    if(cptrig>=ppscw*9 && !(srbits&SRF_REPALL)){
        cpx = CPPOSX;
        cprest = 0; 
        cptrig /= ppscw;

        // t = cptrig / ppscw;
        // d = (cptrig % ppscw) / t;
        // cptrig = t;
        
        // r.topLeft.x = CPPOSX;
        // r.topLeft.y = ppsch-BARHIGHT;
        // r.extent.x = ppscw + d;
        // r.extent.y = CPHIGHT;
        // WinDrawRectangleFrame(rectangleFrame, &r);
    }
    else{
        cptrig = 0xffffffff;        // no progress bar possible
    }
}

//****************************************************************
// 
// QfSetTopLeft
// 
//*****************************************************************

static void QfSetTopLeft(Int16 x)
{
Int16 maxv;
RectangleType rect;

    FldGetBounds(fldptr, &rect);
    rect.topLeft.x = x;
    FldSetBounds(fldptr, &rect);
    FldDrawField(fldptr);
    maxv = ppscw-fieldwidth;
    
    if(x==0 || x==maxv || lasttlx==0 || lasttlx==maxv)
        RefreshMultiControlBitmaps(LEFTRIGHT|BACKERASE);
    
    lasttlx = x;
}

//****************************************************************
// 
// QfGetTopLeft
// 
//*****************************************************************

static Int16 QfGetTopLeft(void)
{
RectangleType rect;

    FldGetBounds(fldptr, &rect);
    return rect.topLeft.x;
}

//****************************************************************
// 
// LstGetCurrentSelText
// 
//*****************************************************************

static Char* LstGetCurrentSelText(ListPtr lstptr)
{
    return LstGetSelectionText(lstptr, LstGetSelection(lstptr));
}

//****************************************************************
// 
// QfSetGetAttr
// 
//*****************************************************************

static Boolean QfSetGetAttr(Int16 v)
{
FieldAttrType attr;

    FldGetAttributes(fldptr, &attr);                
    
    if(v == GETIPVIS){
        return attr.insPtVisible;
    }
    else{
        attr.usable = v;
        FldSetAttributes(fldptr, &attr);                
    }
    
    return false;
}

//*****************************************************************
// 
// JumpLine
// 
//*****************************************************************

static void JumpLine()
{
MemPtr vp;
UInt32 jumpline=0;
UInt16 i;
MemHandle memhand;
MemPtr memptr;
UInt32 l, apos;
Char *p, *end;

    vp = MemHandleLock(gtinfo->texthand);
 
    if(StrLen(vp))
        jumpline = StrAToI(vp);
    
    MemPtrUnlock(vp);

    if(jumpline){
        Field2Recs(false);                                   // save if necessary
        ShowBusy("Counting lines...");
        l=1; apos=0; memptr=0; p=0;

        if(jumpline != 1){    
            for(i=STARTREC; i<=lastrec; i++){
                memhand = DmNewHandle(qedDB, rlen[i]+12);   // oversize
                ErrFatalDisplayIf(memhand==0, "Low Mem!");
                memptr = MemHandleLock(memhand);      
                Rec2Mem(memptr, i, 0, rlen[i]);
                p = memptr;
                end = memptr + rlen[i];
                
                while(p < end)
                    if(*p++ == '\n')
                        if(++l == jumpline)
                            break;
                
                MemPtrFree(memptr);
            
                if(l == jumpline)
                    break;
                
                apos += rlen[i];
            }    
        }
            
        if(l == jumpline){
            apos += (UInt32)p - (UInt32)memptr;
            UpdateField(apos, apos, false);
            FldScrollH(MAXFW, false);
            UpdateScrollBar(true);
        }

        ShowBusy(NULL);
    }
}

//*****************************************************************
// 
// ListKeystroke
// 
//*****************************************************************

static eventsEnum ListKeystroke(Char c, ListPtr listptr)
{
Int16 s, n;
UInt16 i;
eventsEnum ev=nilEvent;

    n = LstGetNumberOfItems(listptr);

    switch(c){
        case chrVerticalTabulation:     // up button      
        case chrFormFeed:               // down button
        case chrRecordSeparator:        // up button
        case chrUnitSeparator:          // down button
            s = LstGetSelection(listptr);
            s += ((c>29 ? c-19 : c)*2 - 23) * (c>29 ? 1 : 9); 
            if(s <= 0) s = 0;
            if(s >= n) s = n-1;
            LstSetSelection(listptr, s);
            break;
    
    case chrLineFeed:                   // return                
        if(listptr == dblstptr)
            CtlHitControl(GetObjectPointer(buttonID_openok));
         else if(listptr == bmlstptr)
            ev = lstSelectEvent;
        
        break;
    
    case chrHorizontalTabulation:       // tab      
        if(listptr == dblstptr)
            CtlHitControl(GetObjectPointer(buttonID_opencancel));
        else if(listptr == bmlstptr)
            ev = frmCloseEvent;
            
        break;
    
    default:
        for(i=0; i<n; i++){             // first char dbname recognizion
            
            if((*LstGetSelectionText(listptr, i) | 0x20) == c){
                s = i - 3;
                LstEraseList(listptr);
                LstSetTopItem(listptr, s<0 ? 0 : s);
                LstSetSelection(listptr, i);
                LstDrawList(listptr);
                break;
            }
        }
    }
    
    return ev;
}

//*****************************************************************
// 
// SwitchFile
// 
//*****************************************************************

static void SwitchFile(void)
{
    if(QedPrefs.twinview)
        StrCopy(QedPrefs.lastdbname[QedPrefs.actdbnanu ^ 0x01], actdbname);
    
    StrCopy(QedPrefs.lastdbname[QedPrefs.actdbnanu], actdbname);
    CloseDB(SAVE_FIELD|SAVE_SPIP|AUTOBUSYOFF);
    
    QedPrefs.actdbnanu ^= 0x01;
    RefreshMultiControlBitmaps(FILEANDTWIN|BACKERASE);
    OpenFirstDB();
}

//*****************************************************************
// 
// DbAttrStatus_CP_RO
// 
//*****************************************************************

static Boolean DbAttrStatus_CP_RO(void)
{
    return DbAttrStatus_Par(dbAttr & (dmHdrAttrCopyPrevention | dmHdrAttrReadOnly));
}

//*****************************************************************
// 
// DbAttrStatus_RO
// 
//*****************************************************************

static Boolean DbAttrStatus_CP(void)
{
    return DbAttrStatus_Par(dbAttr & dmHdrAttrReadOnly);
}

//*****************************************************************
// 
// DbAttrStatus_Par
// 
//*****************************************************************

static Boolean DbAttrStatus_Par(Boolean par)
{
    if(par)
        FrmCustomAlert(alertID_info, "File", " Read", " Only!");
    
    return !par;
}

//*****************************************************************
// 
// Autoindent
// 
//*****************************************************************

static void Autoindent(void)
{
UInt16 ip;
Char *tp, *ep, *z1, *z2;
Boolean lffound;

    ip = FldGetInsPtPosition(fldptr);
    tp = FldGetTextPtr(fldptr);
    z1 = tp + ip - 1;                   // pointer to current position, pos = entered LF

    if(z1 > tp)                         // pos = before entered LF
        z1--;

    while(*z1!=chrLineFeed && z1>tp)    // search for previous end of line
        z1--;
    
    if((lffound = *z1==chrLineFeed) || (z1==tp && frec==1)){    // check finding valid
        ep = tp + FldGetTextLength(fldptr);
        z2 = lffound ? ++z1 : z1;                       
        
        while((*z2==chrSpace || *z2==chrHorizontalTabulation) && z2<ep)
            z2++;
    
        if(z1 != z2)
            FldInsert(fldptr, z1, z2-z1);
    }
}

//*****************************************************************
// 
// TabSpace
// 
//*****************************************************************

static WChar TabSpace(WChar c)
{
Int16 sp, ep, i, sum;
Char *z, *z1, *tp;
UInt8 w, w2;
Char itxt[17]="                ";
WChar cret;
    
    FldGetSelection(fldptr, &sp, &ep);
    cret = c;                                   // assume key not handled

    if(sp == ep){                               // single line TAB insert
        if(c==chrHorizontalTabulation && QedPrefs.tabwidth){
            w = QedPrefs.tabwidth << 2; 
            FldInsert(fldptr, itxt, w-fldptr->insPtXPos%w);
            cret = 0;                           // key handled
        }
    }
    else{                                       // block TAB insert
        tp = FldGetTextPtr(fldptr);
        z = tp + ep-1;                          // pointer to end of selection
        sum = 0;
        
        if(QedPrefs.tabwidth){
            w = QedPrefs.tabwidth << 2;
        }
        else{
            StrCopy(itxt, "\x09");
            w = 1;
        }

        if(c != chrFileSeparator){              // chrHorizontalTabulation||chrGroupSeparator, right key
            for(i=ep; i>=sp; i--){
                if((*z==chrLineFeed && i!=ep) || (z==tp-1 && frabs==0)){
                    FldSetInsertionPoint(fldptr, i);
                    FldInsert(fldptr, itxt, w);
                    sum += w;
                }
              
                z--;
            }
    
        }
        else{                                   // chrFileSeparator, left key            
            w = w - 1;
            
            for(i=ep; i>=sp; i--){                
                if((*z==chrLineFeed && i!=ep) || (z==tp-1 && frabs==0)){
                    z1 = z + 1;
                    w2 = 0;
                    
                    while(*z1++==*itxt && w2++<w)
                        ;
                                        
                    if(w2){
                        FldDelete(fldptr, i, i+w2);
                        sum -= w2;
                    }
                }
              
                z--;
            }
        }

        FldSetSelection(fldptr, sp, ep+sum);
        cret = 0;                               // key handled
    }
    
    return cret;
}

//*****************************************************************
// 
// HandleScreenSlide
// 
//*****************************************************************

static void HandleScreenSlide(Int16 x, Int16 y)
{
static Int16 xo, yo;
Int16 dx, dy, sl, i, fh, f2, fp, ph0inc, ph1inc;
RectangleType rect, erect;
Int16 dir, ctp1_ccp0, cep1, ccp1_ctp0, cep0, etp0;

    if(!(uni1&SCREENSLIDE)){
        FldReleaseFocus(fldptr);            // sidescroll without focus
        xo = x;
        yo = y;
        uni1 |= SCREENSLIDE;
    }
    
    fh = FntLineHeight();
    f2 = fh >> 1;                           // half of font hight
    dx = x - xo; 
    dy = y - yo;
    
    if(fieldwidth>ppscw && abs(dx)>=f2 && abs(dx)>abs(dy)){
        sl = dx - dx % f2;                  // horizontal movement
        FldScrollH(sl, false); 
        xo += sl;
        uni1 &= ~SL_PHASE;                  // reset phase
        yo = y;                             // compensate vertical movement of pen
    }
    else if(abs(dy) >= f2){
        sl = dy / f2;                                           // vertical movement
        FldGetBounds(fldptr, &rect);
        fp = FldGetVisibleLines(fldptr) * fh;                   // fp = (rect.extent.y / fh) * fh;
        erect.topLeft.x = 0;
        erect.extent.x = ppscw;
        xo = x;                                                 // compensate horizontal movement of pen
                
        if(dy < 0){                                             // load parameters for up/down direction  
            uni1 |= SL_DIRDOWN; 
            dir = winDown;                                      // store direction
            ctp1_ccp0 = 1;          // 1 = rect.topLeft.y
            cep1 = fp - f2; 
            ccp1_ctp0 = f2 + 1;
            cep0 = rect.extent.y - f2; 
            etp0 = fp - f2 + 1;

            if(uni1&SL_DIRUP){                                  // dir changed?
                if(uni1&SL_PHASE) FldDrawField(fldptr);         // refresh display
                uni1 &= ~(SL_PHASE | SL_DIRUP);                 // reset phase, dir change handled
            }
        }
        else{
            uni1 |= SL_DIRUP;                                   // store direction
            dir = winUp;
            ctp1_ccp0 = f2 + 1;     // 1 = rect.topLeft.y
            cep1 = rect.extent.y - f2; 
            ccp1_ctp0 = 1;      
            cep0 = fp - f2; 
            etp0 = 1;           
            
            if(uni1&SL_DIRDOWN){                                // dir changed?
                if(uni1&SL_PHASE) FldDrawField(fldptr);         // refresh display
                uni1 &= ~(SL_PHASE | SL_DIRDOWN);               // reset phase, dir change handled
            }
        }

        ph0inc = f2; 
        ph1inc = fh-f2;
        
        if(sl < 0){
            ph0inc = -ph0inc; 
            ph1inc = -ph1inc; 
        }

        for(i=0; i<abs(sl); i++){
            if(FldScrollable(fldptr, dir)){
                if(uni1&SL_PHASE){                              // phase 1, restore shift and scroll
                    erect.topLeft.y = ctp1_ccp0; 
                    erect.extent.y = cep1;
                    WinCopyRectangle(NULL, NULL, &erect, 0, ccp1_ctp0, winPaint);
                    FldScrollField(fldptr, 1, dir);
                    yo += ph1inc;
                }
                else{                                           // phase 0, shift and delete
                    erect.topLeft.y = ccp1_ctp0; 
                    erect.extent.y = cep0;
                    WinCopyRectangle(NULL, NULL, &erect, 0, ctp1_ccp0, winPaint);
                    erect.topLeft.y = etp0; 
                    erect.extent.y = f2;
                    WinEraseRectangle(&erect, 0); 
                    yo += ph0inc;
                }
            }
            else{
                break;
                // FldScrollV(1, dir);
                // yo += ph0inc + ph1inc;
                // uni1 |= SL_PHASE;
            }
        
            uni1 ^= SL_PHASE;                   // alter phase
        }
    }
}

//*****************************************************************
// 
// SizeForm
// 
//*****************************************************************

static void SizeForm(FormPtr frmP)
{
RectangleType rect;
UInt16 i;
Coord conbartop;

    rect.topLeft.x = 0;
    rect.topLeft.y = 0;
    WinGetDisplayExtent(&rect.extent.x, &rect.extent.y);
    ppsch = rect.extent.y;
    ppscw = rect.extent.x;    
    WinSetWindowBounds(FrmGetWindowHandle(frmP), &rect);

    fieldwidth = DocAppInfo.dprop.fiwikey ? DocAppInfo.dprop.fiwikey*320 : ppscw;
    conbartop = ppsch-BARHIGHT;
            
    for(i=0; i<MCBUTNUM; i++)
        SetObjectPosition(FIRSTMCBID+i, BarItem[i].x, conbartop+BarItem[i].y);        

    #ifdef SONY
    if(SilkRefNum){
        QedPrefs.flags2 &= ~DISPLAYEXPAND;  // assume silkResizeNormal
        
        if(ppsch == DEFSCREENHIGHT+65) 
            QedPrefs.flags2 |= silkResizeToStatus<<2;
        else if(ppsch == DEFSCREENHIGHT+80) 
            QedPrefs.flags2 |= silkResizeMax<<2;
    }
    #endif
}

//****************************************************************
// 
// RefreshMultiControlBitmaps
// 
//*****************************************************************

static void RefreshMultiControlBitmaps(UInt8 refresh)
{
Int16 tlx;
Boolean backerase;
    
    if(QedPrefs.baron){
        backerase = refresh & BACKERASE;

        if(refresh&SCROLLDIR)
            DrawBarBitmap(buttonID_scrolldir-FIRSTMCBID, qflags&HSCROLL, backerase);

        if(refresh&LEFTRIGHT){
            tlx = QfGetTopLeft();
            DrawBarBitmap(buttonID_left-FIRSTMCBID, tlx!=0, backerase);
            DrawBarBitmap(buttonID_rigth-FIRSTMCBID, tlx!=ppscw-fieldwidth, backerase);
        }
        
        if(refresh&EDITABLE)
            DrawBarBitmap(buttonID_edit-FIRSTMCBID, editable, backerase);
        
        if(refresh&FILEANDTWIN){
            DrawBarBitmap(buttonID_filex-FIRSTMCBID, QedPrefs.actdbnanu, backerase);
            DrawBarBitmap(TWIN_ID-FIRSTMCBID, QedPrefs.twinview, backerase);
        }
    
        if(refresh&READONLY)
            DrawBarBitmap((dbAttr&dmHdrAttrReadOnly ? READONLY_ID : GRSTIN_ID)-FIRSTMCBID, 0, backerase);
    }
}

//****************************************************************
// 
// DrawBarBitmap
// 
//*****************************************************************

static void DrawBarBitmap(UInt8 index, UInt8 n, Boolean backerase)
{
MemHandle resH;
BitmapType *bitmap;
IndexedColorType prevcol;
RectangleType rect;
#ifdef HIRES
UInt16 oldvalue;
#endif

    resH = DmGetResource(bitmapRsc, FIRSTMCBID+BarItem[index].bitmapid+n);
    if (resH == NULL) return;
    bitmap = MemHandleLock(resH);
    
    #ifdef SONY //--------------------------------------------------------------
        rect.topLeft.x = BarItem[index].x << 1;
        rect.topLeft.y = (ppsch-BARHIGHT+BarItem[index].y) << 1;
    
        if(qflags&COLORMODE && backerase){
            prevcol = WinSetBackColor(barbackcol);
            rect.extent.x = bitmap->width;
            rect.extent.y = bitmap->height;
            HRWinEraseRectangle(HRrefNum, &rect, 0);
        }
        
        HRWinDrawBitmap(HRrefNum, bitmap, rect.topLeft.x, rect.topLeft.y);    
    #else //--------------------------------------------------------------------
        rect.topLeft.x = BarItem[index].x;
        rect.topLeft.y = ppsch-BARHIGHT+BarItem[index].y;

//        #ifdef HIRES
//        if(qflags&HIGHDENSITY){
//            oldvalue = WinSetCoordinateSystem(kCoordinatesNative);
//            rect.topLeft.x <<= 1;
//            rect.topLeft.y <<= 1;
//        }
//        #endif

        if(qflags&COLORMODE && backerase){
            prevcol = WinSetBackColor(barbackcol);
            rect.extent.x = bitmap->width;
            rect.extent.y = bitmap->height;
            WinEraseRectangle(&rect, 0);
        }

        WinDrawBitmap(bitmap, rect.topLeft.x, rect.topLeft.y);    

//        #ifdef HIRES
//        if(qflags&HIGHDENSITY)
//            WinSetCoordinateSystem(oldvalue);
//        #endif
    #endif //-------------------------------------------------------------------

    if(qflags&COLORMODE && backerase)
        WinSetBackColor(prevcol);

    MemHandleUnlock(resH);
    DmReleaseResource (resH);
}

//****************************************************************
// 
// RefreshGSI
// 
//*****************************************************************

static void RefreshGSI(void)
{
UInt8 state;
Boolean capsLock;
UInt16 tempShift;
Boolean optLock;    
Boolean dummy;

    if(qflags&TREOKEYBOARD){
        HsGrfGetStateExt(&capsLock, &dummy, &optLock, &tempShift, &dummy);
        state = tempShift&0x03 | capsLock<<2;
        if(optLock) state = 5;
    }
    else{
        GrfGetState(&capsLock, &dummy, &tempShift, &dummy);
        state = tempShift | capsLock<<2;
        if(state > 4) state = 4;
    }

// if pressing SHIFT repeatedly, it fails on Treo650 after returning to state 0! (in DrawBarBitmap)
// on simulator it works OK :-(
// HEUREKA! it sometimes puts 7 into state... But I don't know why...

    if (state > 5) state = 0;
    
    if(state != laststate) {
        DrawBarBitmap(GRSTIN_ID-FIRSTMCBID, state, true);
    }
//    WinDrawChar ('0'+state, 126, ppsch-BARHIGHT+ICONROW2); // for debugging
        
    laststate = state;

    //              tempShift   capsLock
    // none         0           0       
    // caps lock    0           1       
    // point        1           0       
    // extendshift  2           0       
    // shift        3           0       
                                    
    // 0000    none                    0
    // 0001    point                   1
    // 0010    extshift                2       
    // 0011    shift                   3
    // 0100    capslock                4
    // 0101    capslock+point          5
    // 0110    capslock+extshift       6    
    // 0111    capslock+shift          7
}

//****************************************************************
// 
// BeamDoc
// 
//*****************************************************************

static void BeamDoc(LocalID dbID)
{
ExgSocketType exgSocket;
Char appName[dmDBNameLength+4];
    
    MemSet(&exgSocket, sizeof(exgSocket), 0);     // important to init structure to zeros...
    exgSocket.description = actdbname;
    StrCopy(appName, actdbname);
    StrCat(appName, ".pdb");
    exgSocket.name = appName;

    if(ExgPut(&exgSocket) == errNone){
        ExgDBWrite(WriteProc, &exgSocket, actdbname, dbID, 0);
        ExgDisconnect(&exgSocket, errNone);
    }
}

//****************************************************************
// 
// WriteProc
// 
//*****************************************************************

static Err WriteProc(const void *dataP, UInt32 *sizeP, void *userDataP)
{
Err	err;
	
	*sizeP = ExgSend((ExgSocketPtr)userDataP, dataP, *sizeP, &err);
	return err;
}

/*
//*****************************************************************
// 
// HandleScreenSlide
// simple fonthight step scrolling
// 
//*****************************************************************

static void HandleScreenSlide(Int16 x, Int16 y)
{
static Int16 xo, yo, fh;
Int16 dx, dy, sl, i;
RectangleType rect, erect;

    if(!(uni1&SCREENSLIDE)){
        FldReleaseFocus(fldptr);            // sidescroll without focus
        fh = FntLineHeight();
        xo = x;
        yo = y;
        uni1 |= SCREENSLIDE;
    }

    dx = x - xo;
    dy = y - yo;
    
    if(abs(dx)>=HORSLIDETHRES && abs(dx)>abs(dy)){
        sl = dx - dx % HORSLIDETHRES;
        FldScrollH(sl, false); 
        xo += sl;
    }
    else if(abs(dy) >= fh){
        sl = dy / fh;
        
        for(i=0; i<abs(sl); i++)
            FldScrollField(fldptr, 1, dy<0 ? winDown : winUp);
        
        yo += sl*fh;
    }    
}

//*****************************************************************
// 
// ShowBusy (old version)
// 
//*****************************************************************

static void ShowBusy(Char *title)
{   
static FormPtr curFormP, form;

    if(!(srbits & (SRF_REPALL | SRF_GLOBSEA))){ // no infotexts while global find or replace all
        if(title){
            curFormP = FrmGetActiveForm();      // important!       
            form = FrmInitForm(formID_busy);
            FrmSetTitle(form, title);
            form->window.frameType.bits.cornerDiam = 3;
            FrmSetActiveForm(form);             // important!
            FrmDrawForm(form);
        }
        else{
            FrmEraseForm(form);                 // important!
            FrmDeleteForm(form); 
            FrmSetActiveForm(curFormP);         // important!
        }
    }
}
*/

/* code fragments

UInt16 i, numob;

    numob = FrmGetNumberOfObjects(frmP);
    
    for(i=0; i<numob; i++){
        if(FrmGetObjectType(frmP, i) == frmGraffitiStateObj){
            // FrmGetObjectPosition(frmP, i, &x, &y);
            // GsiSetLocation(x, y+y_diff);
            FrmShowObject(frmP, i);
        }
    }

case menuitemID_quit:
    event->eType = appStopEvent;
    EvtAddEventToQueue(event);
    handled = true;
    break;

// some color ------------------------------------------------------------------

WinScreenMode(winScreenModeGet, 0, 0, &depth, 0);

FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

if (romVersion >= sysVersion35)
		WinPushDrawState();
if (romVersion >= sysVersion35)
	WinPopDrawState();

if (romVersion >= sysVersion35) {
	// Find and cache the indexes in the system palette for the colors we want to draw with.
	whiteIndex = 0;   // White is always 0 in the system palette. 
	
	// black's index changes for different depths (but is always at the highest palette entry)
	MemSet(&rgb, sizeof(rgb), 0);
	blackIndex = WinRGBToIndex(&rgb);

	// We want the board to be green; find the closest index to this shade.
	// (ideally you'd have this shade defined in a resource...)
	// Note that we're also using this shade in grayscale, so don't make it too dark.
	MemSet(&rgb, sizeof(rgb), 0);
	rgb.g=0xa0;
	greenIndex = WinRGBToIndex(&rgb);

	// ditto for red.  The shade is used on the cards, but not in grayscale.
	MemSet(&rgb, sizeof(rgb), 0);
	rgb.r=0xb0;
	redIndex = WinRGBToIndex(&rgb);
}


if(romVersion < sysVersion35) {
    // use the 3.0-3.3 APIs just to draw a gray background
}else {
    // we're not in black and white, and are on a 3.5+ machine; use the good APIs.
    WinSetForeColor(greenIndex);
    WinDrawRectangle(&r, 0);
    WinSetBackColor(whiteIndex);  // get these ready for later
    WinSetForeColor(blackIndex);
}

-----

FtrGet(sysFtrCreator, sysFtrNumDisplayDepth, &defaultdepth);

-----

//****************************************************************
// 
// SetStandardFont
// 
//*****************************************************************

Boolean SetStandardFont(UInt16 index, UInt16 srcindex, UInt32 version)
{
MemHandle fh;
MemPtr ptr;
UInt16 OS4fonts[8]={0x7d00,0x7d01,0x7d02,0x2710,0x2711,0x2712,0x2713,0x07d03};
UInt16 resindex;

	if(version < 0x04000000l)
		resindex = 0x2328+srcindex;
	else
		resindex = OS4fonts[srcindex];
 
	if(fh=DmGetResource('NFNT', resindex)){
		ptr = MemHandleLock(fh);
		SetCustomFont(index, ptr, version);
		MemHandleUnlock(fh);
		DmReleaseResource(fh);
	}
	return true;
}

//****************************************************************
// 
// SetStandardFont
// This routing restores system default font
// 
//*****************************************************************

static void SetStandardFont(UInt16 index)
{
UInt16 OS4fonts[MAXFONTS]={0x7d00,0x7d01,0x7d02,0x2710,0x2711,0x2712,0x2713,0x07d03};
MemHandle fh;

    if((fh=DmGetResource('NFNT',0x2328+index))){
        SetFont(index, fh);
        DmReleaseResource(fh);
    }
}

//****************************************************************
// 
// SetFont
// This routine sets user-defined font from VoidHandle
// handle should be locked
//
//*****************************************************************

static void SetFont(UInt16 index, MemHandle fh)
{
void *fp;
    
    if(fh && MemHandleLockCount(fh) && (fp=MemHandleLock(fh))){
        if(osversion < 0x03000000l)
            *(((UInt32 *)0x1d2)+index) = (UInt32)fp;
        else
            *((*((UInt32 **)0x1da))+index) = (UInt32)fp;
       
        MemHandleUnlock(fh);    
    }
}

//****************************************************************
// 
// HandleDoubleTap
// 
//*****************************************************************

static Boolean HandleDoubleTap(UInt8 mode)
{
static UInt32 tlast;
static UInt16 ilast;
UInt32 ti;
UInt16 ip;
Int16 d;
Char *tp, *z1, *z2;

    ti = TimGetTicks();
    ip = FldGetInsPtPosition(fldptr);
    
    if(ti-tlast <= 40){
        if(mode == DT_TEXTMARK){
            d = ip-ilast;
            d = d<0 ? -d : d;

            if(d <= 1){
                tp = FldGetTextPtr(fldptr);
                
                z1 = z2 = tp + ip;
                
                if(CharValid(*z1) && CharValid(*(z1-1))){
                    while(CharValid(*z1) && z1>=tp)
                        z1--;    
                       
                    while(CharValid(*z2))
                        z2++;
                    
                    FldSetSelection(fldptr, z1-tp+1, z2-tp);
                    FldCopy(fldptr);
                }            
                else{
                    FldPaste(fldptr);
                    CheckField();
                }
                
                ti = 0;
                ip = 0;
            }   
        }
        else if(mode == DT_FILELOAD){
            return true;
        }
    }
    
    tlast = ti;
    ilast = ip;
    return false;
}

----- custom fonts for OS5

fntAppFontCustomBase

static void ReplaceFont(Char* name, UInt16 fid, UInt8 n)
{
LocalID dbid;
DmOpenRef openref;
MemHandle memhandle;
MemPtr ptr;
UInt16 i;    
    
MemHandle memhandle2;
MemPtr ptrf;
    
    if(dbid=DmFindDatabase(0, name)){
        if(openref=DmOpenDatabase(0, dbid, dmModeReadWrite)){    // dmModeReadOnly
            for(i=0; i<NUMRESINDB; i++){
                if(memhandle=DmQueryRecord(openref, i)){
                    if(ptr=MemHandleLock(memhandle)){
                        
                        memhandle2 = DmQueryRecord(openref, i+1);
                        ptrf = MemHandleLock(DmNewHandle(qedDB, MemHandleSize(memhandle) + MemHandleSize(memhandle2)));
                        DmWrite(ptrf, 0, ptr, MemHandleSize(memhandle));
                        MemPtrUnlock(ptr);
                        ptr = MemHandleLock(memhandle2);
                        DmWrite(ptrf, MemHandleSize(memhandle), ptr, MemHandleSize(memhandle2));
                        MemPtrUnlock(ptr);
                        ptr = ptrf;
                        
                        repfont[n].id = fid;
                        repfont[n].sysptr = SetCustomFont(fid, ptr);
                    }
                }
    
                n += 2;         // for Sony hires fonts
                fid += 8;
            }

            DmCloseDatabase(openref);
        }
    }
}

static MemPtr SetCustomFont(UInt16 index, MemPtr ptr)
{
UInt32 oldptr;
UInt32 *p;

	if(!ptr)
		return 0;

	//if(index>7 || (osversion<0x03000000l&&index>6))
	//  index=0;

    if(osversion < 0x05003000){
    	if(osversion < 0x03000000l)
    	    p = ((UInt32 *)0x1d2) + index;
    	else
    		p = (*((UInt32 **)0x1da)) + index;

    	oldptr = *p;
	    *p = (UInt32)ptr;
    }    
    else{
        FntDefineFont(index, (FontType*)ptr);
        oldptr = (UInt32)ptr;
    }
	
	return (MemPtr)oldptr;
}

StopApplication

    for(i=0; i<NUMREPFONTS*NUMRESINDB; i++){
       if(repfont[i].sysptr){
            if(repfont[i].id < fntAppFontCustomBase)
                repptr = SetCustomFont(repfont[i].id, repfont[i].sysptr);
            else
                repptr = repfont[i].sysptr;
                
            MemPtrUnlock(repptr);
        }
    }
*/


/* Treo
#include <HsExt.h>
UInt32 keyboardType;

error = FtrGet (hsFtrCreator, hsFtrIDTypeOfKeyboard, &keyboardType);

if (!error && (keyboardType & hsFtrValKeyboardQwerty))
    hasQwerty = true;

if (!error && (keyboardType & hsFtrValKeyboardJog))
    hasJog = true;
        
Err HsGrfGetStateExt (Boolean* capsLockP, Boolean* numLockP, Boolean* optLockP, UInt16* tempShiftP, Boolean* autoShiftedP)    
*/
