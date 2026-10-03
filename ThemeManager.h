#pragma once

using namespace System;
using namespace System::Drawing;
using namespace Microsoft::Win32;

namespace SchoolCore
{
    public enum class ThemeMode
    {
        System,
        Light,
        Dark
    };

    public ref class ThemeManager abstract sealed
    {
    private:
        literal String^ RegistryPath = L"Software\\SchoolCore\\Appearance";
        literal String^ RegistryValue = L"ThemeMode";

        static ThemeMode currentMode = ThemeMode::System;

        static bool IsSystemDark()
        {
            try
            {
                Object^ value =
                    Registry::CurrentUser->OpenSubKey(
                        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize")
                    ->GetValue(L"AppsUseLightTheme", 1);

                return Convert::ToInt32(value) == 0;
            }
            catch (...)
            {
                return false;
            }
        }

        static bool IsDark()
        {
            if (currentMode == ThemeMode::Dark)
                return true;

            if (currentMode == ThemeMode::Light)
                return false;

            return IsSystemDark();
        }

        static void ApplyControl(System::Windows::Forms::Control^ control, bool dark)
        {
            if (control == nullptr)
                return;

            Color background = dark
                ? Color::FromArgb(15, 23, 42)
                : Color::White;

            Color surface = dark
                ? Color::FromArgb(30, 41, 59)
                : Color::FromArgb(248, 250, 252);

            Color text = dark
                ? Color::FromArgb(226, 232, 240)
                : Color::FromArgb(15, 23, 42);

            Color muted = dark
                ? Color::FromArgb(148, 163, 184)
                : Color::FromArgb(71, 85, 105);

            if (dynamic_cast<System::Windows::Forms::Form^>(control) != nullptr)
            {
                control->BackColor = background;
                control->ForeColor = text;
            }
            else if (dynamic_cast<System::Windows::Forms::Panel^>(control) != nullptr ||
                     dynamic_cast<TableLayoutSystem::Windows::Forms::Panel^>(control) != nullptr ||
                     dynamic_cast<FlowLayoutSystem::Windows::Forms::Panel^>(control) != nullptr ||
                     dynamic_cast<System::Windows::Forms::GroupBox^>(control) != nullptr)
            {
                control->BackColor = surface;
                control->ForeColor = text;
            }
            else if (dynamic_cast<System::Windows::Forms::Label^>(control) != nullptr)
            {
                control->ForeColor = muted;
                if (control->BackColor != Color::Transparent)
                    control->BackColor = Color::Transparent;
            }
            else if (dynamic_cast<System::Windows::Forms::Button^>(control) != nullptr)
            {
                System::Windows::Forms::Button^ button = safe_cast<System::Windows::Forms::Button^>(control);
                button->BackColor = surface;
                button->ForeColor = text;
                button->FlatAppearance->MouseOverBackColor =
                    dark ? Color::FromArgb(51, 65, 85) : Color::FromArgb(241, 245, 249);
                button->FlatAppearance->MouseDownBackColor =
                    dark ? Color::FromArgb(71, 85, 105) : Color::FromArgb(226, 232, 240);
            }
            else if (dynamic_cast<System::Windows::Forms::TextBox^>(control) != nullptr ||
                     dynamic_cast<System::Windows::Forms::ComboBox^>(control) != nullptr ||
                     dynamic_cast<RichSystem::Windows::Forms::TextBox^>(control) != nullptr)
            {
                control->BackColor = dark
                    ? Color::FromArgb(30, 41, 59)
                    : Color::White;
                control->ForeColor = text;
            }
            else if (dynamic_cast<System::Windows::Forms::CheckBox^>(control) != nullptr ||
                     dynamic_cast<RadioSystem::Windows::Forms::Button^>(control) != nullptr)
            {
                control->BackColor = surface;
                control->ForeColor = text;
            }
            else if (dynamic_cast<System::Windows::Forms::DataGridView^>(control) != nullptr)
            {
                System::Windows::Forms::DataGridView^ grid = safe_cast<System::Windows::Forms::DataGridView^>(control);
                grid->BackgroundColor = background;
                grid->GridColor = dark
                    ? Color::FromArgb(71, 85, 105)
                    : Color::FromArgb(226, 232, 240);
                grid->DefaultCellStyle->BackColor = dark
                    ? Color::FromArgb(30, 41, 59)
                    : Color::White;
                grid->DefaultCellStyle->ForeColor = text;
                grid->DefaultCellStyle->SelectionBackColor =
                    dark ? Color::FromArgb(51, 65, 85) : Color::FromArgb(219, 234, 254);
                grid->DefaultCellStyle->SelectionForeColor = text;
                grid->ColumnHeadersDefaultCellStyle->BackColor = dark
                    ? Color::FromArgb(15, 23, 42)
                    : Color::FromArgb(241, 245, 249);
                grid->ColumnHeadersDefaultCellStyle->ForeColor = text;
            }
            else
            {
                control->ForeColor = text;
            }

            for each (System::Windows::Forms::Control^ child in control->Controls)
                ApplyControl(child, dark);
        }

    public:
        static void Load()
        {
            try
            {
                RegistryKey^ key =
                    Registry::CurrentUser->OpenSubKey(RegistryPath);

                if (key == nullptr)
                {
                    currentMode = ThemeMode::System;
                    return;
                }

                String^ value =
                    Convert::ToString(key->GetValue(RegistryValue, L"System"));

                if (value->Equals(L"Dark", StringComparison::OrdinalIgnoreCase))
                    currentMode = ThemeMode::Dark;
                else if (value->Equals(L"Light", StringComparison::OrdinalIgnoreCase))
                    currentMode = ThemeMode::Light;
                else
                    currentMode = ThemeMode::System;
            }
            catch (...)
            {
                currentMode = ThemeMode::System;
            }
        }

        static void Save()
        {
            try
            {
                RegistryKey^ key =
                    Registry::CurrentUser->CreateSubKey(RegistryPath);

                String^ value = L"System";

                if (currentMode == ThemeMode::Dark)
                    value = L"Dark";
                else if (currentMode == ThemeMode::Light)
                    value = L"Light";

                key->SetValue(RegistryValue, value);
            }
            catch (...)
            {
            }
        }

        static ThemeMode GetMode()
        {
            return currentMode;
        }

        static void SetMode(ThemeMode mode)
        {
            currentMode = mode;
            Save();
        }

        static String^ GetModeName()
        {
            if (currentMode == ThemeMode::Dark)
                return L"Dark";

            if (currentMode == ThemeMode::Light)
                return L"Light";

            return L"System";
        }

        static void ApplyToForm(System::Windows::Forms::Form^ form)
        {
            if (form == nullptr)
                return;

            ApplyControl(form, IsDark());
            form->Invalidate(true);
            form->Update();
        }

        static void ApplyToOpenForms()
        {
            for each (System::Windows::Forms::Form^ form in System::Windows::Forms::Application::OpenForms)
                ApplyToForm(form);
        }
    };
}
