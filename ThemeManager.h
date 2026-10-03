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
        static ThemeMode currentMode = ThemeMode::System;

    public:
        static void Load()
        {
            currentMode = ThemeMode::System;
        }

        static void Save()
        {
        }

        static ThemeMode GetMode()
        {
            return currentMode;
        }

        static void SetMode(ThemeMode mode)
        {
            currentMode = mode;
        }

        static System::String^ GetModeName()
        {
            return L"Light";
        }

        static void ApplyToForm(System::Windows::Forms::Form^ form)
        {
            if (form == nullptr)
                return;

            form->BackColor = System::Drawing::Color::White;
            form->ForeColor = System::Drawing::Color::Black;
        }

        static void ApplyToOpenForms()
        {
            for each (System::Windows::Forms::Form^ form
                     in System::Windows::Forms::Application::OpenForms)
            {
                if (form != nullptr)
                {
                    form->BackColor = System::Drawing::Color::White;
                    form->ForeColor = System::Drawing::Color::Black;
                }
            }
        }
    };
}
