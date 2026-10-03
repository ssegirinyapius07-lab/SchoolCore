#pragma once

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
        literal System::String^ RegistryPath =
            L"Software\\SchoolCore\\Appearance";

        literal System::String^ RegistryValue =
            L"ThemeMode";

        static ThemeMode currentMode = ThemeMode::System;

        static bool IsSystemDark()
        {
            try
            {
                Microsoft::Win32::RegistryKey^ personalize =
                    Microsoft::Win32::Registry::CurrentUser->OpenSubKey(
                        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");

                if (personalize == nullptr)
                    return false;

                System::Object^ value =
                    personalize->GetValue(L"AppsUseLightTheme", 1);

                return System::Convert::ToInt32(value) == 0;
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

        static void ApplyControl(
            System::Windows::Forms::Control^ control,
            bool dark)
        {
            if (control == nullptr)
                return;

            System::Drawing::Color background =
                dark
                    ? System::Drawing::Color::FromArgb(15, 23, 42)
                    : System::Drawing::Color::White;

            System::Drawing::Color surface =
                dark
                    ? System::Drawing::Color::FromArgb(30, 41, 59)
                    : System::Drawing::Color::FromArgb(248, 250, 252);

            System::Drawing::Color text =
                dark
                    ? System::Drawing::Color::FromArgb(226, 232, 240)
                    : System::Drawing::Color::FromArgb(15, 23, 42);

            System::Drawing::Color muted =
                dark
                    ? System::Drawing::Color::FromArgb(148, 163, 184)
                    : System::Drawing::Color::FromArgb(71, 85, 105);

            if (dynamic_cast<System::Windows::Forms::Form^>(control) != nullptr)
            {
                control->BackColor = background;
                control->ForeColor = text;
            }
            else if (dynamic_cast<System::Windows::Forms::Panel^>(control) != nullptr ||
                     dynamic_cast<System::Windows::Forms::GroupBox^>(control) != nullptr)
            {
                control->BackColor = surface;
                control->ForeColor = text;
            }
            else if (dynamic_cast<System::Windows::Forms::Label^>(control) != nullptr)
            {
                control->ForeColor = muted;
                if (control->BackColor != System::Drawing::Color::Transparent)
                    control->BackColor = System::Drawing::Color::Transparent;
            }
            else if (dynamic_cast<System::Windows::Forms::Button^>(control) != nullptr)
            {
                System::Windows::Forms::Button^ button =
                    safe_cast<System::Windows::Forms::Button^>(control);

                button->BackColor = surface;
                button->ForeColor = text;
                button->FlatAppearance->MouseOverBackColor =
                    dark
                        ? System::Drawing::Color::FromArgb(51, 65, 85)
                        : System::Drawing::Color::FromArgb(241, 245, 249);

                button->FlatAppearance->MouseDownBackColor =
                    dark
                        ? System::Drawing::Color::FromArgb(71, 85, 105)
                        : System::Drawing::Color::FromArgb(226, 232, 240);
            }
            else if (dynamic_cast<System::Windows::Forms::TextBox^>(control) != nullptr ||
                     dynamic_cast<System::Windows::Forms::ComboBox^>(control) != nullptr)
            {
                control->BackColor =
                    dark
                        ? System::Drawing::Color::FromArgb(30, 41, 59)
                        : System::Drawing::Color::White;

                control->ForeColor = text;
            }
            else if (dynamic_cast<System::Windows::Forms::CheckBox^>(control) != nullptr ||
                     dynamic_cast<System::Windows::Forms::RadioButton^>(control) != nullptr)
            {
                control->BackColor = surface;
                control->ForeColor = text;
            }
            else if (dynamic_cast<System::Windows::Forms::DataGridView^>(control) != nullptr)
            {
                System::Windows::Forms::DataGridView^ grid =
                    safe_cast<System::Windows::Forms::DataGridView^>(control);

                grid->BackgroundColor = background;
                grid->GridColor =
                    dark
                        ? System::Drawing::Color::FromArgb(71, 85, 105)
                        : System::Drawing::Color::FromArgb(226, 232, 240);

                grid->DefaultCellStyle->BackColor =
                    dark
                        ? System::Drawing::Color::FromArgb(30, 41, 59)
                        : System::Drawing::Color::White;

                grid->DefaultCellStyle->ForeColor = text;

                grid->DefaultCellStyle->SelectionBackColor =
                    dark
                        ? System::Drawing::Color::FromArgb(51, 65, 85)
                        : System::Drawing::Color::FromArgb(219, 234, 254);

                grid->DefaultCellStyle->SelectionForeColor = text;

                grid->ColumnHeadersDefaultCellStyle->BackColor =
                    dark
                        ? System::Drawing::Color::FromArgb(15, 23, 42)
                        : System::Drawing::Color::FromArgb(241, 245, 249);

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
                Microsoft::Win32::RegistryKey^ key =
                    Microsoft::Win32::Registry::CurrentUser->OpenSubKey(RegistryPath);

                if (key == nullptr)
                {
                    currentMode = ThemeMode::System;
                    return;
                }

                System::String^ value =
                    System::Convert::ToString(
                        key->GetValue(RegistryValue, L"System"));

                if (System::String::Equals(
                        value,
                        L"Dark",
                        System::StringComparison::OrdinalIgnoreCase))
                {
                    currentMode = ThemeMode::Dark;
                }
                else if (System::String::Equals(
                             value,
                             L"Light",
                             System::StringComparison::OrdinalIgnoreCase))
                {
                    currentMode = ThemeMode::Light;
                }
                else
                {
                    currentMode = ThemeMode::System;
                }
            }
            catch (...)
            {
                currentMode = ThemeMode::System;
            }
        }

        static void Save()
        {
            Microsoft::Win32::RegistryKey^ key = nullptr;

            try
            {
                key =
                    Microsoft::Win32::Registry::CurrentUser->CreateSubKey(
                        RegistryPath);

                if (key == nullptr)
                    return;

                System::String^ value = L"System";

                if (currentMode == ThemeMode::Dark)
                    value = L"Dark";
                else if (currentMode == ThemeMode::Light)
                    value = L"Light";

                key->SetValue(RegistryValue, value);
            }
            catch (...)
            {
            }

            if (key != nullptr)
                delete key;
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

        static System::String^ GetModeName()
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
            for each (System::Windows::Forms::Form^ form
                     in System::Windows::Forms::Application::OpenForms)
            {
                ApplyToForm(form);
            }
        }
    };
}
