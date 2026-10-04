#pragma once

namespace SchoolCore
{
    public enum class ThemeMode
    {
        System,
        Light,
        Dark
    };

    // =========================================================
    // SchoolCore design system
    //
    // One neutral palette and a single accent colour, applied
    // to every form that calls ThemeManager::ApplyToForm.
    //
    // A control (or a whole panel) can opt out by setting its
    // Tag to the text "NoTheme".
    // =========================================================
    public ref class ThemeManager abstract sealed
    {
    private:
        static ThemeMode currentMode = ThemeMode::System;

        static bool IsOptedOut(System::Windows::Forms::Control^ control)
        {
            System::String^ tag =
                dynamic_cast<System::String^>(control->Tag);

            return tag != nullptr &&
                   tag->Equals(L"NoTheme");
        }

        static bool IsFilledColor(System::Drawing::Color c)
        {
            if (c.IsEmpty || c.IsSystemColor || c.A != 255)
            {
                return false;
            }

            return c.GetBrightness() < 0.78f;
        }

        static bool IsDangerHue(System::Drawing::Color c)
        {
            return c.R > 150 && c.G < 110 && c.B < 110;
        }

        static System::Drawing::Color Darken(System::Drawing::Color c)
        {
            return System::Drawing::Color::FromArgb(
                (int)(c.R * 0.88),
                (int)(c.G * 0.88),
                (int)(c.B * 0.88)
            );
        }

        // Role names are kept in Button::Tag, but only when the form
        // has not already used the Tag for its own data.
        static void RememberRole(
            System::Windows::Forms::Button^ button,
            System::String^ role)
        {
            if (button->Tag == nullptr ||
                dynamic_cast<System::String^>(button->Tag) != nullptr)
            {
                System::String^ existing =
                    dynamic_cast<System::String^>(button->Tag);

                if (existing == nullptr ||
                    existing->StartsWith(L"Theme"))
                {
                    button->Tag = role;
                }
            }
        }

        static void ApplyButtonState(System::Windows::Forms::Button^ button)
        {
            System::String^ role =
                dynamic_cast<System::String^>(button->Tag);

            if (role == nullptr || !role->StartsWith(L"Theme"))
            {
                return;
            }

            if (!button->Enabled)
            {
                button->BackColor = System::Drawing::Color::FromArgb(241, 245, 249);
                button->ForeColor = System::Drawing::Color::FromArgb(148, 163, 184);
                button->FlatAppearance->BorderSize = 1;
                button->FlatAppearance->BorderColor =
                    System::Drawing::Color::FromArgb(226, 232, 240);
                return;
            }

            if (role->Equals(L"ThemePrimary"))
            {
                button->BackColor = System::Drawing::Color::FromArgb(29, 78, 216);
                button->ForeColor = System::Drawing::Color::White;
                button->FlatAppearance->BorderSize = 0;
            }
            else if (role->Equals(L"ThemeDanger"))
            {
                button->BackColor = System::Drawing::Color::FromArgb(185, 28, 28);
                button->ForeColor = System::Drawing::Color::White;
                button->FlatAppearance->BorderSize = 0;
            }
            else
            {
                button->BackColor = System::Drawing::Color::FromArgb(226, 232, 240);
                button->ForeColor = System::Drawing::Color::FromArgb(15, 23, 42);
                button->FlatAppearance->BorderSize = 1;
                button->FlatAppearance->BorderColor =
                    System::Drawing::Color::FromArgb(148, 163, 184);
            }
        }

        static void ButtonEnabledChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            System::Windows::Forms::Button^ button =
                dynamic_cast<System::Windows::Forms::Button^>(sender);

            if (button != nullptr)
            {
                ApplyButtonState(button);
            }
        }

        static void WatchButton(System::Windows::Forms::Button^ button)
        {
            System::EventHandler^ handler =
                gcnew System::EventHandler(
                    &ThemeManager::ButtonEnabledChanged
                );

            button->EnabledChanged -= handler;
            button->EnabledChanged += handler;
        }

    public:
        // -----------------------------------------------------
        // Palette
        // -----------------------------------------------------

        static System::Drawing::Color Canvas()
        {
            return System::Drawing::Color::FromArgb(248, 250, 252);
        }

        static System::Drawing::Color Surface()
        {
            return System::Drawing::Color::White;
        }

        static System::Drawing::Color Ink()
        {
            return System::Drawing::Color::FromArgb(15, 23, 42);
        }

        static System::Drawing::Color TextSecondary()
        {
            return System::Drawing::Color::FromArgb(71, 85, 105);
        }

        static System::Drawing::Color TextMuted()
        {
            return System::Drawing::Color::FromArgb(100, 116, 139);
        }

        static System::Drawing::Color Border()
        {
            return System::Drawing::Color::FromArgb(203, 213, 225);
        }

        static System::Drawing::Color Divider()
        {
            return System::Drawing::Color::FromArgb(226, 232, 240);
        }

        static System::Drawing::Color HeaderFill()
        {
            return System::Drawing::Color::FromArgb(241, 245, 249);
        }

        static System::Drawing::Color BorderStrong()
        {
            return System::Drawing::Color::FromArgb(148, 163, 184);
        }

        static System::Drawing::Color DisabledFill()
        {
            return System::Drawing::Color::FromArgb(241, 245, 249);
        }

        static System::Drawing::Color DisabledText()
        {
            return System::Drawing::Color::FromArgb(148, 163, 184);
        }

        static System::Drawing::Color Selection()
        {
            return System::Drawing::Color::FromArgb(219, 234, 254);
        }

        static System::Drawing::Color Accent()
        {
            return System::Drawing::Color::FromArgb(29, 78, 216);
        }

        static System::Drawing::Color AccentHover()
        {
            return System::Drawing::Color::FromArgb(30, 64, 175);
        }

        static System::Drawing::Color Danger()
        {
            return System::Drawing::Color::FromArgb(185, 28, 28);
        }

        static System::Drawing::Color Success()
        {
            return System::Drawing::Color::FromArgb(21, 128, 61);
        }

        // -----------------------------------------------------
        // Mode handling (unchanged behaviour)
        // -----------------------------------------------------

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

        // -----------------------------------------------------
        // Control styling helpers
        // -----------------------------------------------------

        static void StyleTextBox(System::Windows::Forms::TextBox^ box)
        {
            box->BorderStyle =
                System::Windows::Forms::BorderStyle::FixedSingle;

            box->ForeColor = Ink();

            box->BackColor =
                box->ReadOnly
                ? HeaderFill()
                : Surface();
        }

        static void StyleComboBox(System::Windows::Forms::ComboBox^ combo)
        {
            combo->FlatStyle =
                System::Windows::Forms::FlatStyle::Flat;

            combo->ForeColor = Ink();
            combo->BackColor = Surface();
        }

        static void StylePrimaryButton(System::Windows::Forms::Button^ button)
        {
            button->FlatStyle =
                System::Windows::Forms::FlatStyle::Flat;

            button->FlatAppearance->MouseOverBackColor = AccentHover();
            button->FlatAppearance->MouseDownBackColor = AccentHover();
            button->Cursor = System::Windows::Forms::Cursors::Hand;

            RememberRole(button, L"ThemePrimary");
            WatchButton(button);
            ApplyButtonState(button);
        }

        static void StyleDangerButton(System::Windows::Forms::Button^ button)
        {
            button->FlatStyle =
                System::Windows::Forms::FlatStyle::Flat;

            button->FlatAppearance->MouseOverBackColor = Darken(Danger());
            button->FlatAppearance->MouseDownBackColor = Darken(Danger());
            button->Cursor = System::Windows::Forms::Cursors::Hand;

            RememberRole(button, L"ThemeDanger");
            WatchButton(button);
            ApplyButtonState(button);
        }

        static void StyleSecondaryButton(System::Windows::Forms::Button^ button)
        {
            button->FlatStyle =
                System::Windows::Forms::FlatStyle::Flat;

            button->FlatAppearance->MouseOverBackColor = Border();
            button->FlatAppearance->MouseDownBackColor = BorderStrong();
            button->Cursor = System::Windows::Forms::Cursors::Hand;

            RememberRole(button, L"ThemeSecondary");
            WatchButton(button);
            ApplyButtonState(button);
        }

        static void StyleGrid(System::Windows::Forms::DataGridView^ grid)
        {
            grid->BackgroundColor = Surface();
            grid->BorderStyle =
                System::Windows::Forms::BorderStyle::None;

            grid->GridColor = Divider();

            grid->CellBorderStyle =
                System::Windows::Forms::DataGridViewCellBorderStyle::SingleHorizontal;

            grid->EnableHeadersVisualStyles = false;

            grid->ColumnHeadersBorderStyle =
                System::Windows::Forms::DataGridViewHeaderBorderStyle::Single;

            grid->ColumnHeadersDefaultCellStyle->BackColor = HeaderFill();
            grid->ColumnHeadersDefaultCellStyle->ForeColor = Ink();
            grid->ColumnHeadersDefaultCellStyle->SelectionBackColor = HeaderFill();
            grid->ColumnHeadersDefaultCellStyle->SelectionForeColor = Ink();
            grid->ColumnHeadersDefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.0F,
                    System::Drawing::FontStyle::Bold
                );

            grid->DefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    grid->DefaultCellStyle->Font != nullptr
                        ? grid->DefaultCellStyle->Font->SizeInPoints
                        : 9.0F,
                    System::Drawing::FontStyle::Bold
                );

            grid->DefaultCellStyle->BackColor = Surface();
            grid->DefaultCellStyle->ForeColor = Ink();
            grid->DefaultCellStyle->SelectionBackColor = Selection();
            grid->DefaultCellStyle->SelectionForeColor = Ink();

            grid->ColumnHeadersHeightSizeMode =
                System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::DisableResizing;
            grid->ColumnHeadersHeight = 40;
            grid->RowTemplate->Height = 34;

            grid->ColumnHeadersDefaultCellStyle->Padding =
                System::Windows::Forms::Padding(8, 0, 8, 0);
            grid->DefaultCellStyle->Padding =
                System::Windows::Forms::Padding(8, 0, 8, 0);

            grid->AlternatingRowsDefaultCellStyle->BackColor = Canvas();
            grid->AlternatingRowsDefaultCellStyle->ForeColor = Ink();
            grid->AlternatingRowsDefaultCellStyle->SelectionBackColor = Selection();
            grid->AlternatingRowsDefaultCellStyle->SelectionForeColor = Ink();
        }

        // Buttons that already carry a deliberate fill are mapped
        // onto the single accent (or the danger colour for red
        // buttons) so the application stops mixing many colours.
        static void NormaliseButton(System::Windows::Forms::Button^ button)
        {
            if (button->FlatStyle !=
                System::Windows::Forms::FlatStyle::Flat)
            {
                StyleSecondaryButton(button);
                return;
            }

            System::Drawing::Color fill = button->BackColor;

            if (!IsFilledColor(fill))
            {
                return;
            }

            if (IsDangerHue(fill))
            {
                StyleDangerButton(button);
            }
            else
            {
                StylePrimaryButton(button);
            }
        }

        // -----------------------------------------------------
        // Applying the theme
        // -----------------------------------------------------

        static void ApplyBoldTypographyToForm(
            System::Windows::Forms::Form^ form)
        {
            if (form == nullptr)
            {
                return;
            }

            if (form->Font != nullptr)
            {
                form->Font =
                    gcnew System::Drawing::Font(
                        L"Segoe UI",
                        form->Font->SizeInPoints,
                        System::Drawing::FontStyle::Bold
                    );
            }

            ApplyBoldTypography(form->Controls);
        }

        static void ApplyBoldTypography(
            System::Windows::Forms::Control::ControlCollection^ controls)
        {
            if (controls == nullptr)
            {
                return;
            }

            for each (System::Windows::Forms::Control^ control in controls)
            {
                if (control == nullptr)
                {
                    continue;
                }

                if (control->Font != nullptr)
                {
                    control->Font =
                        gcnew System::Drawing::Font(
                            L"Segoe UI",
                            control->Font->SizeInPoints,
                            System::Drawing::FontStyle::Bold
                        );
                }

                System::Windows::Forms::DataGridView^ grid =
                    dynamic_cast<System::Windows::Forms::DataGridView^>(
                        control);

                if (grid != nullptr)
                {
                    if (grid->ColumnHeadersDefaultCellStyle->Font != nullptr)
                    {
                        grid->ColumnHeadersDefaultCellStyle->Font =
                            gcnew System::Drawing::Font(
                                L"Segoe UI",
                                grid->ColumnHeadersDefaultCellStyle->Font->SizeInPoints,
                                System::Drawing::FontStyle::Bold
                            );
                    }

                    if (grid->DefaultCellStyle->Font != nullptr)
                    {
                        grid->DefaultCellStyle->Font =
                            gcnew System::Drawing::Font(
                                L"Segoe UI",
                                grid->DefaultCellStyle->Font->SizeInPoints,
                                System::Drawing::FontStyle::Bold
                            );
                    }

                    if (grid->AlternatingRowsDefaultCellStyle->Font != nullptr)
                    {
                        grid->AlternatingRowsDefaultCellStyle->Font =
                            gcnew System::Drawing::Font(
                                L"Segoe UI",
                                grid->AlternatingRowsDefaultCellStyle->Font->SizeInPoints,
                                System::Drawing::FontStyle::Bold
                            );
                    }

                    for each (
                        System::Windows::Forms::DataGridViewColumn^ column
                        in grid->Columns)
                    {
                        if (column->DefaultCellStyle->Font != nullptr)
                        {
                            column->DefaultCellStyle->Font =
                                gcnew System::Drawing::Font(
                                    L"Segoe UI",
                                    column->DefaultCellStyle->Font->SizeInPoints,
                                    System::Drawing::FontStyle::Bold
                                );
                        }
                    }
                }

                if (control->HasChildren)
                {
                    ApplyBoldTypography(control->Controls);
                }
            }
        }

        static void ApplyToControls(
            System::Windows::Forms::Control::ControlCollection^ controls)
        {
            if (controls == nullptr)
            {
                return;
            }

            for each (System::Windows::Forms::Control^ control in controls)
            {
                if (control == nullptr)
                {
                    continue;
                }

                // Keep the existing font size and visual scale, but make
                // application text consistently heavier and easier to read.
                if (control->Font != nullptr)
                {
                    float size = control->Font->SizeInPoints;

                    System::Windows::Forms::Label^ label =
                        dynamic_cast<System::Windows::Forms::Label^>(control);

                    System::Windows::Forms::Button^ buttonForFont =
                        dynamic_cast<System::Windows::Forms::Button^>(control);

                    System::Windows::Forms::CheckBox^ checkBox =
                        dynamic_cast<System::Windows::Forms::CheckBox^>(control);

                    System::Windows::Forms::RadioButton^ radioButton =
                        dynamic_cast<System::Windows::Forms::RadioButton^>(control);

                    System::Windows::Forms::LinkLabel^ linkLabel =
                        dynamic_cast<System::Windows::Forms::LinkLabel^>(control);

                    System::Windows::Forms::GroupBox^ groupBox =
                        dynamic_cast<System::Windows::Forms::GroupBox^>(control);

                    System::Drawing::FontStyle style =
                        System::Drawing::FontStyle::Regular;

                    if (label != nullptr ||
                        buttonForFont != nullptr ||
                        checkBox != nullptr ||
                        radioButton != nullptr ||
                        linkLabel != nullptr ||
                        groupBox != nullptr)
                    {
                        style = System::Drawing::FontStyle::Bold;
                    }
                    else
                    {
                        style = System::Drawing::FontStyle::Bold;
                    }

                    control->Font =
                        gcnew System::Drawing::Font(
                            L"Segoe UI",
                            size,
                            style
                        );
                }

                System::Windows::Forms::TextBox^ box =
                    dynamic_cast<System::Windows::Forms::TextBox^>(control);

                if (box != nullptr)
                {
                    StyleTextBox(box);
                    continue;
                }

                System::Windows::Forms::ComboBox^ combo =
                    dynamic_cast<System::Windows::Forms::ComboBox^>(control);

                if (combo != nullptr)
                {
                    StyleComboBox(combo);
                    continue;
                }

                System::Windows::Forms::DataGridView^ grid =
                    dynamic_cast<System::Windows::Forms::DataGridView^>(control);

                if (grid != nullptr)
                {
                    StyleGrid(grid);
                    continue;
                }

                System::Windows::Forms::Button^ button =
                    dynamic_cast<System::Windows::Forms::Button^>(control);

                if (button != nullptr)
                {
                    NormaliseButton(button);
                    continue;
                }

                if (control->HasChildren)
                {
                    ApplyToControls(control->Controls);
                }
            }
        }

        // For controls created after the form has been built.
        static void ApplyToControl(System::Windows::Forms::Control^ control)
        {
            if (control == nullptr)
            {
                return;
            }

            if (control->Font != nullptr)
            {
                float size = control->Font->SizeInPoints;

                control->Font =
                    gcnew System::Drawing::Font(
                        L"Segoe UI",
                        size,
                        System::Drawing::FontStyle::Bold
                    );
            }

            System::Windows::Forms::TextBox^ box =
                dynamic_cast<System::Windows::Forms::TextBox^>(control);

            if (box != nullptr)
            {
                StyleTextBox(box);
                return;
            }

            System::Windows::Forms::ComboBox^ combo =
                dynamic_cast<System::Windows::Forms::ComboBox^>(control);

            if (combo != nullptr)
            {
                StyleComboBox(combo);
                return;
            }

            System::Windows::Forms::DataGridView^ grid =
                dynamic_cast<System::Windows::Forms::DataGridView^>(control);

            if (grid != nullptr)
            {
                StyleGrid(grid);
                return;
            }

            System::Windows::Forms::Button^ button =
                dynamic_cast<System::Windows::Forms::Button^>(control);

            if (button != nullptr)
            {
                NormaliseButton(button);
                return;
            }

            if (control->HasChildren)
            {
                ApplyToControls(control->Controls);
            }
        }

        static void ApplyToForm(System::Windows::Forms::Form^ form)
        {
            if (form == nullptr)
            {
                return;
            }

            System::String^ name = form->GetType()->Name;

            form->BackColor = Canvas();
            form->ForeColor = Ink();

            // Typography is global, including hand-styled forms/dialogs.
            // Colours and layout are still allowed to remain custom.
            ApplyBoldTypographyToForm(form);

            // The dashboard uses a dark navigation rail that must
            // keep its own colours.
            if (name->Equals(L"Dashboard"))
            {
                return;
            }

            if (name->Equals(L"LoginForm") ||
                name->Equals(L"PasswordChangeForm"))
            {
                return;
            }

            ApplyToControls(form->Controls);
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
