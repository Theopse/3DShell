#include <algorithm>

#include "c2d_helper.h"
#include "colours.h"
#include "config.h"
#include "fs.h"
#include "gui.h"
#include "net.h"
#include "textures.h"
#include "utils.h"

namespace GUI {
    enum SETTINGS_STATE {
        GENERAL_SETTINGS,
        SORT_SETTINGS,
        UPDATE_SETTINGS
    };
    
    static SETTINGS_STATE settings_state = GENERAL_SETTINGS;
    static int selection = 0;
    static const int sel_dist = 40;
    static std::string tag_name = std::string();
    static bool network_status = false, update_available = false, update_popup = false;

    static void DisplaySortSettings(void) {
        C2D::Text(10, 28, 0.44f, WHITE, "Sorting Options");
        
        C2D::Text(10, 58, 0.44f, cfg.dark_theme? WHITE : BLACK, "字母顺序");
        C2D::Text(10, 74, 0.42f, cfg.dark_theme? WHITE : BLACK, "按A-Z顺序排列");
        C2D::Text(10, 98, 0.44f, cfg.dark_theme? WHITE : BLACK, "字母顺序");
        C2D::Text(10, 114, 0.42f, cfg.dark_theme? WHITE : BLACK, "按Z-A顺序排列");
        C2D::Text(10, 138, 0.44f, cfg.dark_theme? WHITE : BLACK, "大小");
        C2D::Text(10, 154, 0.42f, cfg.dark_theme? WHITE : BLACK, "按大-小排列");
        C2D::Text(10, 178, 0.44f, cfg.dark_theme? WHITE : BLACK, "大小");
        C2D::Text(10, 194, 0.42f, cfg.dark_theme? WHITE : BLACK, "按小-大排列");

        C2D::Image(cfg.sort == 0? (cfg.dark_theme? icon_radio_dark_on : icon_radio_on) : (cfg.dark_theme? icon_radio_dark_off : icon_radio_off), 270, 60);
        C2D::Image(cfg.sort == 1? (cfg.dark_theme? icon_radio_dark_on : icon_radio_on) : (cfg.dark_theme? icon_radio_dark_off : icon_radio_off), 270, 100);
        C2D::Image(cfg.sort == 2? (cfg.dark_theme? icon_radio_dark_on : icon_radio_on) : (cfg.dark_theme? icon_radio_dark_off : icon_radio_off), 270, 140);
        C2D::Image(cfg.sort == 3? (cfg.dark_theme? icon_radio_dark_on : icon_radio_on) : (cfg.dark_theme? icon_radio_dark_off : icon_radio_off), 270, 180);
    }

    static void ControlSortSettings(MenuItem *item, u32 *kDown) {
        if (*kDown & KEY_DUP)
            selection--;
        else if (*kDown & KEY_DDOWN)
            selection++;
        else if (*kDown & KEY_A) {
            cfg.sort = selection;
            Config::Save(cfg);
            FS::GetDirList(cfg.cwd, item->entries);
        }
        else if (*kDown & KEY_B) {
            selection = 0;
            settings_state = GENERAL_SETTINGS;
        }

        Utils::SetBounds(&selection, 0, 3);
    }

    static void DisplayUpdateSettings(void) {
        C2D::Text(10, 28, 0.44f, WHITE, "Updates");

        C2D::Text(10, 58, 0.44f, cfg.dark_theme? WHITE : BLACK, "检查更新");
        C2D::Text(10, 74, 0.42f, cfg.dark_theme? WHITE : BLACK, "下载并安装最新版本");
        C2D::Text(10, 98, 0.44f, cfg.dark_theme? WHITE : BLACK, "关于");
        C2D::Textf(10, 114, 0.42f, cfg.dark_theme? WHITE : BLACK, "3DShell v%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
        C2D::Text(10, 138, 0.44f, cfg.dark_theme? WHITE : BLACK, "作者: Joel16");
        C2D::Text(10, 154, 0.42f, cfg.dark_theme? WHITE : BLACK, "资源: Preetisketch/CyanogenMod/LineageOS");
        C2D::Text(10, 170, 0.42f, cfg.dark_theme? WHITE : BLACK, "汉化: Theopse/Advance汉化组");

        if (update_popup)
            GUI::DisplayUpdateOptions(&network_status, &update_available, tag_name);
    }

    static void ControlUpdateSettings(MenuItem *item, u32 *kDown) {
        if (update_popup)
            GUI::ControlUpdateOptions(item, kDown, &update_popup, &network_status, &update_available, tag_name);
        else {
            if (*kDown & KEY_DUP)
                selection--;
            else if (*kDown & KEY_DDOWN)
                selection++;
            else if (*kDown & KEY_A) {
                if (selection == 0) {
                    Net::Init();
                    network_status =  Net::GetNetworkStatus();
                    tag_name = Net::GetLatestReleaseJSON();
                    update_available = Net::GetAvailableUpdate(tag_name);
                    update_popup = true;
                    Net::Exit();
                }
            }
            else if (*kDown & KEY_B) {
                selection = 0;
                settings_state = GENERAL_SETTINGS;
            }
            
            Utils::SetBounds(&selection, 0, 2);
        }
    }

    static void DisplayGeneralSettings(void) {
        C2D::Text(10, 28, 0.44f, WHITE, "Settings");

        C2D::Text(10, 58, 0.44f, cfg.dark_theme? WHITE : BLACK, "文件排序");
        C2D::Text(10, 74, 0.42f, cfg.dark_theme? WHITE : BLACK, "在各种排序选项之间选择");
        C2D::Text(10, 98, 0.44f, cfg.dark_theme? WHITE : BLACK, "深色主题");
        C2D::Text(10, 114, 0.42f, cfg.dark_theme? WHITE : BLACK, "启用深色主题");
        C2D::Text(10, 138, 0.44f, cfg.dark_theme? WHITE : BLACK, "开发者选项");
        C2D::Text(10, 154, 0.42f, cfg.dark_theme? WHITE : BLACK, "启用日志记录和fs访问NAND");
        C2D::Text(10, 178, 0.44f, cfg.dark_theme? WHITE : BLACK, "检查更新");
        C2D::Text(10, 194, 0.42f, cfg.dark_theme? WHITE : BLACK, "下载并安装最新版本");
        
        if (cfg.dark_theme)
            C2D::Image(cfg.dark_theme? icon_toggle_dark_on : icon_toggle_on, 270, 97);
        else
            C2D::Image(icon_toggle_off, 270, 97);

        C2D::Image(cfg.dev_options? (cfg.dark_theme? icon_toggle_dark_on : icon_toggle_on) : icon_toggle_off, 270, 137);
    }

    void ControlGeneralSettings(MenuItem *item, u32 *kDown) {
        if (*kDown & KEY_DUP)
            selection--;
        else if (*kDown & KEY_DDOWN)
            selection++;

        if (*kDown & KEY_A) {
            switch(selection) {
                case 0:
                    settings_state = SORT_SETTINGS;
                    selection = 0;
                    break;

                case 1:
                    cfg.dark_theme = !cfg.dark_theme;
                    Config::Save(cfg);
                    break;
                
                case 2:
                    cfg.dev_options = !cfg.dev_options;
                    Config::Save(cfg);
                    break;

                case 3:
                    settings_state = UPDATE_SETTINGS;
                    selection = 0;
                    break;
            }
        }
        else if (*kDown & KEY_B)
            item->state = MENU_STATE_FILEBROWSER;

        Utils::SetBounds(&selection, 0, 3);
    }

    void DisplaySettings(MenuItem *item) {
        C2D::Rect(0, 20, 400, 35, cfg.dark_theme? MENU_BAR_DARK : STATUS_BAR_LIGHT); // Menu bar
        C2D::Rect(0, 55, 320, 185, cfg.dark_theme? BLACK_BG : WHITE);

        C2D::Rect(0, 55 + (selection * sel_dist), 320, sel_dist, cfg.dark_theme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);

        switch(settings_state) {
            case GENERAL_SETTINGS:
                DisplayGeneralSettings();
                break;
            
            case SORT_SETTINGS:
                DisplaySortSettings();
                break;
            
            case UPDATE_SETTINGS:
                DisplayUpdateSettings();
                break;
        }
    }

    void ControlSettings(MenuItem *item, u32 *kDown) {
        switch(settings_state) {
            case GENERAL_SETTINGS:
                ControlGeneralSettings(item, kDown);
                break;
            
            case SORT_SETTINGS:
                ControlSortSettings(item, kDown);
                break;
            
            case UPDATE_SETTINGS:
                ControlUpdateSettings(item, kDown);
                break;
        }
    }
}
