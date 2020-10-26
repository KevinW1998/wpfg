#include <AVUICommon.h>
#include <AVUIKeyInterop.h>

namespace AVUI
{

Key::Enum KeyInterop::KeyFromVirtualKey(int virtualKey)
{
    switch (virtualKey)
    {
        case 3:
            return Key::Cancel;

        case 8:
            return Key::Back;

        case 9:
            return Key::Tab;

        case 12:
            return Key::Clear;

        case 13:
            return Key::Return;

        case 0x10:
        case 160:
            return Key::LeftShift;

        case 0x11:
        case 0xa2:
            return Key::LeftCtrl;

        case 0x12:
        case 0xa4:
            return Key::LeftAlt;

        case 0x13:
            return Key::Pause;

        case 20:
            return Key::Capital;

        case 0x15:
            return Key::KanaMode;

        case 0x17:
            return Key::JunjaMode;

        case 0x18:
            return Key::FinalMode;

        case 0x19:
            return Key::HanjaMode;

        case 0x1b:
            return Key::Escape;

        case 0x1c:
            return Key::ImeConvert;

        case 0x1d:
            return Key::ImeNonConvert;

        case 30:
            return Key::ImeAccept;

        case 0x1f:
            return Key::ImeModeChange;

        case 0x20:
            return Key::Space;

        case 0x21:
            return Key::Prior;

        case 0x22:
            return Key::Next;

        case 0x23:
            return Key::End;

        case 0x24:
            return Key::Home;

        case 0x25:
            return Key::Left;

        case 0x26:
            return Key::Up;

        case 0x27:
            return Key::Right;

        case 40:
            return Key::Down;

        case 0x29:
            return Key::Select;

        case 0x2a:
            return Key::Print;

        case 0x2b:
            return Key::Execute;

        case 0x2c:
            return Key::Snapshot;

        case 0x2d:
            return Key::Insert;

        case 0x2e:
            return Key::Delete;

        case 0x2f:
            return Key::Help;

        case 0x30:
            return Key::D0;

        case 0x31:
            return Key::D1;

        case 50:
            return Key::D2;

        case 0x33:
            return Key::D3;

        case 0x34:
            return Key::D4;

        case 0x35:
            return Key::D5;

        case 0x36:
            return Key::D6;

        case 0x37:
            return Key::D7;

        case 0x38:
            return Key::D8;

        case 0x39:
            return Key::D9;

        case 0x41:
            return Key::A;

        case 0x42:
            return Key::B;

        case 0x43:
            return Key::C;

        case 0x44:
            return Key::D;

        case 0x45:
            return Key::E;

        case 70:
            return Key::F;

        case 0x47:
            return Key::G;

        case 0x48:
            return Key::H;

        case 0x49:
            return Key::I;

        case 0x4a:
            return Key::J;

        case 0x4b:
            return Key::K;

        case 0x4c:
            return Key::L;

        case 0x4d:
            return Key::M;

        case 0x4e:
            return Key::N;

        case 0x4f:
            return Key::O;

        case 80:
            return Key::P;

        case 0x51:
            return Key::Q;

        case 0x52:
            return Key::R;

        case 0x53:
            return Key::S;

        case 0x54:
            return Key::T;

        case 0x55:
            return Key::U;

        case 0x56:
            return Key::V;

        case 0x57:
            return Key::W;

        case 0x58:
            return Key::X;

        case 0x59:
            return Key::Y;

        case 90:
            return Key::Z;

        case 0x5b:
            return Key::LWin;

        case 0x5c:
            return Key::RWin;

        case 0x5d:
            return Key::Apps;

        case 0x5f:
            return Key::Sleep;

        case 0x60:
            return Key::NumPad0;

        case 0x61:
            return Key::NumPad1;

        case 0x62:
            return Key::NumPad2;

        case 0x63:
            return Key::NumPad3;

        case 100:
            return Key::NumPad4;

        case 0x65:
            return Key::NumPad5;

        case 0x66:
            return Key::NumPad6;

        case 0x67:
            return Key::NumPad7;

        case 0x68:
            return Key::NumPad8;

        case 0x69:
            return Key::NumPad9;

        case 0x6a:
            return Key::Multiply;

        case 0x6b:
            return Key::Add;

        case 0x6c:
            return Key::Separator;

        case 0x6d:
            return Key::Subtract;

        case 110:
            return Key::Decimal;

        case 0x6f:
            return Key::Divide;

        case 0x70:
            return Key::F1;

        case 0x71:
            return Key::F2;

        case 0x72:
            return Key::F3;

        case 0x73:
            return Key::F4;

        case 0x74:
            return Key::F5;

        case 0x75:
            return Key::F6;

        case 0x76:
            return Key::F7;

        case 0x77:
            return Key::F8;

        case 120:
            return Key::F9;

        case 0x79:
            return Key::F10;

        case 0x7a:
            return Key::F11;

        case 0x7b:
            return Key::F12;

        case 0x7c:
            return Key::F13;

        case 0x7d:
            return Key::F14;

        case 0x7e:
            return Key::F15;

        case 0x7f:
            return Key::F16;

        case 0x80:
            return Key::F17;

        case 0x81:
            return Key::F18;

        case 130:
            return Key::F19;

        case 0x83:
            return Key::F20;

        case 0x84:
            return Key::F21;

        case 0x85:
            return Key::F22;

        case 0x86:
            return Key::F23;

        case 0x87:
            return Key::F24;

        case 0x90:
            return Key::NumLock;

        case 0x91:
            return Key::Scroll;

        case 0xa1:
            return Key::RightShift;

        case 0xa3:
            return Key::RightCtrl;

        case 0xa5:
            return Key::RightAlt;

        case 0xa6:
            return Key::BrowserBack;

        case 0xa7:
            return Key::BrowserForward;

        case 0xa8:
            return Key::BrowserRefresh;

        case 0xa9:
            return Key::BrowserStop;

        case 170:
            return Key::BrowserSearch;

        case 0xab:
            return Key::BrowserFavorites;

        case 0xac:
            return Key::BrowserHome;

        case 0xad:
            return Key::VolumeMute;

        case 0xae:
            return Key::VolumeDown;

        case 0xaf:
            return Key::VolumeUp;

        case 0xb0:
            return Key::MediaNextTrack;

        case 0xb1:
            return Key::MediaPreviousTrack;

        case 0xb2:
            return Key::MediaStop;

        case 0xb3:
            return Key::MediaPlayPause;

        case 180:
            return Key::LaunchMail;

        case 0xb5:
            return Key::SelectMedia;

        case 0xb6:
            return Key::LaunchApplication1;

        case 0xb7:
            return Key::LaunchApplication2;

        case 0xba:
            return Key::Oem1;

        case 0xbb:
            return Key::OemPlus;

        case 0xbc:
            return Key::OemComma;

        case 0xbd:
            return Key::OemMinus;

        case 190:
            return Key::OemPeriod;

        case 0xbf:
            return Key::Oem2;

        case 0xc0:
            return Key::Oem3;

        case 0xc1:
            return Key::AbntC1;

        case 0xc2:
            return Key::AbntC2;

        case 0xdb:
            return Key::Oem4;

        case 220:
            return Key::Oem5;

        case 0xdd:
            return Key::Oem6;

        case 0xde:
            return Key::Oem7;

        case 0xdf:
            return Key::Oem8;

        case 0xe2:
            return Key::Oem102;

        case 0xe5:
            return Key::ImeProcessed;

        case 240:
            return Key::OemAttn;

        case 0xf1:
            return Key::OemFinish;

        case 0xf2:
            return Key::OemCopy;

        case 0xf3:
            return Key::OemAuto;

        case 0xf4:
            return Key::OemEnlw;

        case 0xf5:
            return Key::OemBackTab;

        case 0xf6:
            return Key::Attn;

        case 0xf7:
            return Key::CrSel;

        case 0xf8:
            return Key::ExSel;

        case 0xf9:
            return Key::EraseEof;

        case 250:
            return Key::Play;

        case 0xfb:
            return Key::Zoom;

        case 0xfc:
            return Key::NoName;

        case 0xfd:
            return Key::Pa1;

        case 0xfe:
            return Key::OemClear;
    }
    return Key::None;

}



};
