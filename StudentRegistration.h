#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class StudentRegistration : public Form
    {
    private:

        // =========================================================
        // CONTROLS
        // =========================================================

        TableLayoutPanel^ mainLayout;

        Panel^ scrollPanel;
        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;

        GroupBox^ studentGroup;
        GroupBox^ enrollmentGroup;
        GroupBox^ guardianGroup;

        // Student controls
        Label^ lblFirstName;
        Label^ lblMiddleName;
        Label^ lblLastName;
        Label^ lblDob;
        Label^ lblGender;
        Label^ lblAdmissionDate;
        Label^ lblHomeAddress;

        TextBox^ txtFirstName;
        TextBox^ txtMiddleName;
        TextBox^ txtLastName;
        DateTimePicker^ dtpDob;
        ComboBox^ cmbGender;
        DateTimePicker^ dtpAdmissionDate;
        TextBox^ txtHomeAddress;

        // Student photo
        Panel^ photoPanel;
        PictureBox^ picStudentPhoto;
        Button^ btnChoosePhoto;
        Label^ lblPhotoHint;
        String^ selectedPhotoSourcePath = nullptr;
        String^ editingPhotoPath = L"";
        String^ editingRegistrationNumber = L"";

        // Enrollment controls
        Label^ lblAcademicYear;
        Label^ lblTerm;
        Label^ lblClass;
        Label^ lblStream;
        Label^ lblStreamInfo;

        ComboBox^ cmbAcademicYear;
        ComboBox^ cmbTerm;
        ComboBox^ cmbClass;
        ComboBox^ cmbStream;

        // Guardian controls
        Label^ lblGuardianName;
        Label^ lblGuardianRelationship;
        Label^ lblGuardianPhone;
        Label^ lblGuardianAlternativePhone;
        Label^ lblGuardianEmail;
        Label^ lblGuardianAddress;

        TextBox^ txtGuardianName;
        TextBox^ txtGuardianRelationship;
        TextBox^ txtGuardianPhone;
        TextBox^ txtGuardianAlternativePhone;
        TextBox^ txtGuardianEmail;
        TextBox^ txtGuardianAddress;

        // Buttons
        Panel^ buttonPanel;
        Button^ btnSave;
        Button^ btnClear;
        Button^ btnCancel;

        // Edit mode
        bool editMode = false;
        long long editingStudentId = 0;
        long long editingGuardianId = 0;
        int editingEnrollmentId = 0;


        // =========================================================
        // COMBO ITEM
        // =========================================================

        ref class ComboItem
        {
        public:
            int Id;
            String^ Text;
            int Count;

            ComboItem(int id, String^ text)
            {
                Id = id;
                Text = text;
                Count = -1;
            }

            ComboItem(int id, String^ text, int count)
            {
                Id = id;
                Text = text;
                Count = count;
            }

            virtual String^ ToString() override
            {
                if (Count >= 0)
                {
                    return Text + L" (" +
                        Count.ToString() +
                        L" students)";
                }

                return Text;
            }
        };


        // =========================================================
        // STUDENT PHOTO HELPERS
        // =========================================================

        void MakeCircularPictureBox(
            PictureBox^ pictureBox)
        {
            System::Drawing::Drawing2D::GraphicsPath^ path =
                gcnew System::Drawing::Drawing2D::GraphicsPath();

            path->AddEllipse(
                0,
                0,
                pictureBox->Width,
                pictureBox->Height
            );

            pictureBox->Region =
                gcnew System::Drawing::Region(path);
        }


        void ShowPhotoPreview(
            String^ imagePath)
        {
            if (this->picStudentPhoto == nullptr)
            {
                return;
            }

            if (this->picStudentPhoto->Image != nullptr)
            {
                delete this->picStudentPhoto->Image;
                this->picStudentPhoto->Image = nullptr;
            }

            if (
                String::IsNullOrWhiteSpace(imagePath) ||
                !System::IO::File::Exists(imagePath)
            )
            {
                this->lblPhotoHint->Text =
                    L"No photo selected";

                return;
            }

            try
            {
                Image^ loaded =
                    Image::FromFile(imagePath);

                this->picStudentPhoto->Image =
                    gcnew Bitmap(loaded);

                delete loaded;

                this->lblPhotoHint->Text =
                    L"Photo selected";
            }
            catch (System::Exception^)
            {
                this->lblPhotoHint->Text =
                    L"Unable to load photo";
            }
        }


        String^ SaveStudentPhoto(
            String^ sourcePath,
            String^ registrationNumber)
        {
            if (
                String::IsNullOrWhiteSpace(sourcePath)
            )
            {
                return L"";
            }

            String^ folder =
                System::IO::Path::Combine(
                    Application::StartupPath,
                    L"StudentPhotos"
                );

            System::IO::Directory::CreateDirectory(
                folder
            );

            String^ extension =
                System::IO::Path::GetExtension(
                    sourcePath
                );

            if (
                String::IsNullOrWhiteSpace(extension)
            )
            {
                extension = L".jpg";
            }

            String^ fileName =
                registrationNumber->Replace(
                    L"/",
                    L"_"
                ) +
                extension->ToLowerInvariant();

            String^ destination =
                System::IO::Path::Combine(
                    folder,
                    fileName
                );

            String^ sourceFull =
                System::IO::Path::GetFullPath(
                    sourcePath
                );

            String^ destinationFull =
                System::IO::Path::GetFullPath(
                    destination
                );

            if (
                !sourceFull->Equals(
                    destinationFull,
                    StringComparison::OrdinalIgnoreCase
                )
            )
            {
                System::IO::File::Copy(
                    sourcePath,
                    destination,
                    true
                );
            }

            return
                System::IO::Path::Combine(
                    L"StudentPhotos",
                    fileName
                );
        }


        // =========================================================
        // INITIALIZE COMPONENTS
        // =========================================================

        void InitializeComponent()
        {
            this->SuspendLayout();

            // =========================================================
            // FORM
            // =========================================================

            this->Text = L"Student Registration";
            this->StartPosition = FormStartPosition::CenterParent;
            this->WindowState = FormWindowState::Maximized;
            this->ClientSize = System::Drawing::Size(1100, 760);
            this->MinimumSize = System::Drawing::Size(900, 650);

            this->BackColor = Color::WhiteSmoke;

            this->Font = gcnew System::Drawing::Font(
                L"Segoe UI",
                9.5F,
                FontStyle::Regular
            );


            // =========================================================
            // SCROLL PANEL
            // =========================================================

            this->scrollPanel = gcnew Panel();

            this->scrollPanel->Dock = DockStyle::Fill;
            this->scrollPanel->AutoScroll = true;
            this->scrollPanel->BackColor = Color::WhiteSmoke;
            this->scrollPanel->Padding =
                System::Windows::Forms::Padding(20);

            this->scrollPanel->AutoScrollMinSize =
                System::Drawing::Size(
                    0,
                    900
                );


            // =========================================================
            // MAIN LAYOUT
            // =========================================================

            this->mainLayout = gcnew TableLayoutPanel();

            this->mainLayout->Dock = DockStyle::Top;
            this->mainLayout->AutoSize = true;
            this->mainLayout->AutoSizeMode =
                System::Windows::Forms::AutoSizeMode::GrowAndShrink;

            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 5;

            this->mainLayout->BackColor =
                Color::WhiteSmoke;

            this->mainLayout->Margin =
                System::Windows::Forms::Padding(0);


            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );


            // Header
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    78.0F
                )
            );

            // Student
            // Extra vertical space keeps the address field and photo
            // section comfortably inside the Student Information box.
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    285.0F
                )
            );

            // Enrollment
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    165.0F
                )
            );

            // Guardian
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    210.0F
                )
            );

            // Buttons
            // A larger action row keeps all actions reachable when
            // the registration form is reduced and scrolled.
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    70.0F
                )
            );


            // =========================================================
            // HEADER
            // =========================================================

            this->headerPanel = gcnew Panel();

            this->headerPanel->Dock = DockStyle::Fill;

            this->headerPanel->BackColor =
                Color::FromArgb(35, 47, 62);

            this->headerPanel->Padding =
                System::Windows::Forms::Padding(
                    20, 10, 20, 10
                );


            this->lblTitle = gcnew Label();

            this->lblTitle->AutoSize = true;

            this->lblTitle->Text =
                L"Student Registration";

            this->lblTitle->ForeColor =
                Color::White;

            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F,
                    FontStyle::Bold
                );

            this->lblTitle->Location =
                Point(18, 10);


            this->lblSubtitle = gcnew Label();

            this->lblSubtitle->AutoSize = true;

            this->lblSubtitle->Text =
                L"Register a new student and assign the initial enrollment.";

            this->lblSubtitle->ForeColor =
                Color::FromArgb(220, 225, 230);

            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Regular
                );

            this->lblSubtitle->Location =
                Point(20, 45);


            this->headerPanel->Controls->Add(
                this->lblTitle
            );

            this->headerPanel->Controls->Add(
                this->lblSubtitle
            );


            // =========================================================
            // STUDENT GROUP
            // =========================================================

            this->studentGroup =
                gcnew GroupBox();

            this->studentGroup->Dock =
                DockStyle::Fill;

            this->studentGroup->Text =
                L"Student Information";

            this->studentGroup->Padding =
                System::Windows::Forms::Padding(12);

            this->studentGroup->Margin =
                System::Windows::Forms::Padding(
                    0, 0, 0, 10
                );

            this->studentGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    FontStyle::Bold
                );


            TableLayoutPanel^ studentLayout =
                gcnew TableLayoutPanel();

            studentLayout->Dock =
                DockStyle::Fill;

            studentLayout->ColumnCount = 4;
            studentLayout->RowCount = 4;

            studentLayout->Padding =
                System::Windows::Forms::Padding(5);


            // Label columns
            studentLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    125.0F
                )
            );

            // Input columns
            studentLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    50.0F
                )
            );

            // Label columns
            studentLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    125.0F
                )
            );

            // Input columns
            studentLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    50.0F
                )
            );


            for (int i = 0; i < 3; i++)
            {
                studentLayout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        42.0F
                    )
                );
            }

            studentLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    80.0F
                )
            );


            // ---------------------------------------------------------
            // First Name
            // ---------------------------------------------------------

            this->lblFirstName =
                gcnew Label();

            this->lblFirstName->Text =
                L"First Name";

            this->lblFirstName->Dock =
                DockStyle::Fill;

            this->lblFirstName->AutoSize = false;

            this->lblFirstName->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblFirstName->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->txtFirstName =
                gcnew TextBox();

            this->txtFirstName->Dock =
                DockStyle::Fill;

            this->txtFirstName->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);


            // ---------------------------------------------------------
            // Last Name
            // ---------------------------------------------------------

            this->lblLastName =
                gcnew Label();

            this->lblLastName->Text =
                L"Last Name";

            this->lblLastName->Dock =
                DockStyle::Fill;

            this->lblLastName->AutoSize = false;

            this->lblLastName->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblLastName->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->txtLastName =
                gcnew TextBox();

            this->txtLastName->Dock =
                DockStyle::Fill;

            this->txtLastName->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);


            // ---------------------------------------------------------
            // Middle Name
            // ---------------------------------------------------------

            this->lblMiddleName =
                gcnew Label();

            this->lblMiddleName->Text =
                L"Middle Name";

            this->lblMiddleName->Dock =
                DockStyle::Fill;

            this->lblMiddleName->AutoSize = false;

            this->lblMiddleName->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblMiddleName->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->txtMiddleName =
                gcnew TextBox();

            this->txtMiddleName->Dock =
                DockStyle::Fill;

            this->txtMiddleName->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);


            // ---------------------------------------------------------
            // Gender
            // ---------------------------------------------------------

            this->lblGender =
                gcnew Label();

            this->lblGender->Text =
                L"Gender";

            this->lblGender->Dock =
                DockStyle::Fill;

            this->lblGender->AutoSize = false;

            this->lblGender->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblGender->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->cmbGender =
                gcnew ComboBox();

            this->cmbGender->Dock =
                DockStyle::Fill;

            this->cmbGender->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->cmbGender->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);

            this->cmbGender->Items->Add(
                L"Select Gender"
            );

            this->cmbGender->Items->Add(
                L"Male"
            );

            this->cmbGender->Items->Add(
                L"Female"
            );

            this->cmbGender->SelectedIndex = 0;


            // ---------------------------------------------------------
            // Date of Birth
            // ---------------------------------------------------------

            this->lblDob =
                gcnew Label();

            this->lblDob->Text =
                L"Date of Birth";

            this->lblDob->Dock =
                DockStyle::Fill;

            this->lblDob->AutoSize = false;

            this->lblDob->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblDob->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->dtpDob =
                gcnew DateTimePicker();

            this->dtpDob->Dock =
                DockStyle::Fill;

            this->dtpDob->AutoSize = false;

            this->dtpDob->Width = 140;
            this->dtpDob->Height = 28;

            this->dtpDob->Format =
                DateTimePickerFormat::Custom;

            this->dtpDob->CustomFormat =
                L"dd/MM/yyyy";

            this->dtpDob->MaxDate =
                DateTime::Today;

            this->dtpDob->Value =
                DateTime::Today.AddYears(-15);

            this->dtpDob->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);

            this->dtpDob->Anchor =
                AnchorStyles::Left |
                AnchorStyles::Right;


            // ---------------------------------------------------------
            // Admission Date
            // ---------------------------------------------------------

            this->lblAdmissionDate =
                gcnew Label();

            this->lblAdmissionDate->Text =
                L"Admission Date";

            this->lblAdmissionDate->Dock =
                DockStyle::Fill;

            this->lblAdmissionDate->AutoSize = false;

            this->lblAdmissionDate->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblAdmissionDate->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->dtpAdmissionDate =
                gcnew DateTimePicker();

            this->dtpAdmissionDate->Dock =
                DockStyle::Fill;

            this->dtpAdmissionDate->AutoSize = false;

            this->dtpAdmissionDate->Width = 140;
            this->dtpAdmissionDate->Height = 28;

            this->dtpAdmissionDate->Format =
                DateTimePickerFormat::Custom;

            this->dtpAdmissionDate->CustomFormat =
                L"dd/MM/yyyy";

            this->dtpAdmissionDate->MaxDate =
                DateTime::Today;

            this->dtpAdmissionDate->Value =
                DateTime::Today;

            this->dtpAdmissionDate->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);

            this->dtpAdmissionDate->Anchor =
                AnchorStyles::Left |
                AnchorStyles::Right;


            // ---------------------------------------------------------
            // Home Address
            // ---------------------------------------------------------

            this->lblHomeAddress =
                gcnew Label();

            this->lblHomeAddress->Text =
                L"Home Address";

            this->lblHomeAddress->Dock =
                DockStyle::Fill;

            this->lblHomeAddress->AutoSize = false;

            this->lblHomeAddress->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblHomeAddress->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->txtHomeAddress =
                gcnew TextBox();

            this->txtHomeAddress->Dock =
                DockStyle::Fill;

            this->txtHomeAddress->Multiline =
                true;

            this->txtHomeAddress->ScrollBars =
                ScrollBars::Vertical;

            this->txtHomeAddress->Margin =
                System::Windows::Forms::Padding(3, 5, 3, 5);


            // ---------------------------------------------------------
            // Add Student Controls
            // ---------------------------------------------------------

            studentLayout->Controls->Add(
                this->lblFirstName,
                0, 0
            );

            studentLayout->Controls->Add(
                this->txtFirstName,
                1, 0
            );

            studentLayout->Controls->Add(
                this->lblLastName,
                2, 0
            );

            studentLayout->Controls->Add(
                this->txtLastName,
                3, 0
            );


            studentLayout->Controls->Add(
                this->lblMiddleName,
                0, 1
            );

            studentLayout->Controls->Add(
                this->txtMiddleName,
                1, 1
            );

            studentLayout->Controls->Add(
                this->lblGender,
                2, 1
            );

            studentLayout->Controls->Add(
                this->cmbGender,
                3, 1
            );


            studentLayout->Controls->Add(
                this->lblDob,
                0, 2
            );

            studentLayout->Controls->Add(
                this->dtpDob,
                1, 2
            );

            studentLayout->Controls->Add(
                this->lblAdmissionDate,
                2, 2
            );

            studentLayout->Controls->Add(
                this->dtpAdmissionDate,
                3, 2
            );


            studentLayout->Controls->Add(
                this->lblHomeAddress,
                0, 3
            );

            studentLayout->Controls->Add(
                this->txtHomeAddress,
                1, 3
            );

            studentLayout->SetColumnSpan(
                this->txtHomeAddress,
                3
            );


            // The student fields and photo are kept in separate
            // layout columns so the photo never overlaps any field.
            TableLayoutPanel^ studentSectionLayout =
                gcnew TableLayoutPanel();

            studentSectionLayout->Dock =
                DockStyle::Fill;

            studentSectionLayout->ColumnCount = 2;
            studentSectionLayout->RowCount = 1;

            studentSectionLayout->Padding =
                System::Windows::Forms::Padding(4);

            studentSectionLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            studentSectionLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    175.0F
                )
            );

            studentSectionLayout->Controls->Add(
                studentLayout,
                0,
                0
            );


            // ---------------------------------------------------------
            // Student Photo
            // ---------------------------------------------------------

            this->photoPanel =
                gcnew Panel();

            this->photoPanel->Dock =
                DockStyle::Fill;

            this->photoPanel->Margin =
                System::Windows::Forms::Padding(
                    8,
                    10,
                    8,
                    10
                );

            this->photoPanel->BorderStyle =
                BorderStyle::FixedSingle;

            this->photoPanel->Padding =
                System::Windows::Forms::Padding(
                    8,
                    10,
                    8,
                    10
                );

            this->picStudentPhoto =
                gcnew PictureBox();

            this->picStudentPhoto->Size =
                System::Drawing::Size(
                    100,
                    100
                );

            this->picStudentPhoto->Location =
                System::Drawing::Point(
                    28,
                    10
                );

            this->picStudentPhoto->SizeMode =
                PictureBoxSizeMode::Zoom;

            this->picStudentPhoto->BackColor =
                Color::Gainsboro;

            this->picStudentPhoto->BorderStyle =
                BorderStyle::FixedSingle;



            this->btnChoosePhoto =
                gcnew Button();

            this->btnChoosePhoto->Text =
                L"Choose Photo";

            this->btnChoosePhoto->Size =
                System::Drawing::Size(
                    105,
                    30
                );

            this->btnChoosePhoto->Location =
                System::Drawing::Point(
                    26,
                    123
                );

            this->btnChoosePhoto->FlatStyle =
                FlatStyle::Flat;

            this->btnChoosePhoto->FlatAppearance->BorderSize =
                0;

            this->btnChoosePhoto->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->btnChoosePhoto->ForeColor =
                Color::White;


            this->lblPhotoHint =
                gcnew Label();

            this->lblPhotoHint->Text =
                L"No photo selected";

            this->lblPhotoHint->AutoSize =
                false;

            this->lblPhotoHint->Size =
                System::Drawing::Size(
                    125,
                    32
                );

            this->lblPhotoHint->Location =
                System::Drawing::Point(
                    4,
                    162
                );

            this->lblPhotoHint->TextAlign =
                ContentAlignment::TopCenter;

            this->lblPhotoHint->ForeColor =
                Color::DimGray;


            this->photoPanel->Controls->Add(
                this->lblPhotoHint
            );

            this->photoPanel->Controls->Add(
                this->btnChoosePhoto
            );

            this->photoPanel->Controls->Add(
                this->picStudentPhoto
            );

            studentSectionLayout->Controls->Add(
                this->photoPanel,
                1,
                0
            );

            this->studentGroup->Controls->Add(
                studentSectionLayout
            );


            // =========================================================
            // ENROLLMENT GROUP
            // =========================================================

            this->enrollmentGroup =
                gcnew GroupBox();

            this->enrollmentGroup->Dock =
                DockStyle::Fill;

            this->enrollmentGroup->Text =
                L"Enrollment Information";

            this->enrollmentGroup->Padding =
                System::Windows::Forms::Padding(
                    12, 10, 12, 10
                );

            this->enrollmentGroup->Margin =
                System::Windows::Forms::Padding(
                    0, 0, 0, 10
                );

            this->enrollmentGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    FontStyle::Bold
                );


            TableLayoutPanel^ enrollmentLayout =
                gcnew TableLayoutPanel();

            enrollmentLayout->Dock =
                DockStyle::Fill;

            enrollmentLayout->ColumnCount = 4;
            enrollmentLayout->RowCount = 3;

            enrollmentLayout->Padding =
                System::Windows::Forms::Padding(5);


            enrollmentLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    125.0F
                )
            );

            enrollmentLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    50.0F
                )
            );

            enrollmentLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    125.0F
                )
            );

            enrollmentLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    50.0F
                )
            );


            enrollmentLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    40.0F
                )
            );

            enrollmentLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    40.0F
                )
            );

            enrollmentLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    34.0F
                )
            );


            // Academic Year

            this->lblAcademicYear =
                gcnew Label();

            this->lblAcademicYear->Text =
                L"Academic Year";

            this->lblAcademicYear->Dock =
                DockStyle::Fill;

            this->lblAcademicYear->AutoSize = false;

            this->lblAcademicYear->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblAcademicYear->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->cmbAcademicYear =
                gcnew ComboBox();

            this->cmbAcademicYear->Dock =
                DockStyle::Fill;

            this->cmbAcademicYear->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->cmbAcademicYear->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);


            // Term

            this->lblTerm =
                gcnew Label();

            this->lblTerm->Text =
                L"Term";

            this->lblTerm->Dock =
                DockStyle::Fill;

            this->lblTerm->AutoSize = false;

            this->lblTerm->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblTerm->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->cmbTerm =
                gcnew ComboBox();

            this->cmbTerm->Dock =
                DockStyle::Fill;

            this->cmbTerm->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->cmbTerm->Enabled =
                false;

            this->cmbTerm->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);


            // Class

            this->lblClass =
                gcnew Label();

            this->lblClass->Text =
                L"Class";

            this->lblClass->Dock =
                DockStyle::Fill;

            this->lblClass->AutoSize = false;

            this->lblClass->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblClass->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->cmbClass =
                gcnew ComboBox();

            this->cmbClass->Dock =
                DockStyle::Fill;

            this->cmbClass->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->cmbClass->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);


            // Stream

            this->lblStream =
                gcnew Label();

            this->lblStream->Text =
                L"Stream";

            this->lblStream->Dock =
                DockStyle::Fill;

            this->lblStream->AutoSize = false;

            this->lblStream->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblStream->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);


            this->cmbStream =
                gcnew ComboBox();

            this->cmbStream->Dock =
                DockStyle::Fill;

            this->cmbStream->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->cmbStream->Enabled =
                false;

            this->cmbStream->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 4);


            // Stream Info

            this->lblStreamInfo =
                gcnew Label();

            this->lblStreamInfo->AutoSize =
                false;

            this->lblStreamInfo->Dock =
                DockStyle::Fill;

            this->lblStreamInfo->Text =
                L"Select an academic year, term and class "
                L"to view stream enrollment.";

            this->lblStreamInfo->ForeColor =
                Color::DimGray;

            this->lblStreamInfo->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    8.5F,
                    FontStyle::Italic
                );

            this->lblStreamInfo->TextAlign =
                ContentAlignment::MiddleLeft;


            enrollmentLayout->Controls->Add(
                this->lblAcademicYear,
                0, 0
            );

            enrollmentLayout->Controls->Add(
                this->cmbAcademicYear,
                1, 0
            );

            enrollmentLayout->Controls->Add(
                this->lblTerm,
                2, 0
            );

            enrollmentLayout->Controls->Add(
                this->cmbTerm,
                3, 0
            );


            enrollmentLayout->Controls->Add(
                this->lblClass,
                0, 1
            );

            enrollmentLayout->Controls->Add(
                this->cmbClass,
                1, 1
            );

            enrollmentLayout->Controls->Add(
                this->lblStream,
                2, 1
            );

            enrollmentLayout->Controls->Add(
                this->cmbStream,
                3, 1
            );


            enrollmentLayout->Controls->Add(
                this->lblStreamInfo,
                0, 2
            );

            enrollmentLayout->SetColumnSpan(
                this->lblStreamInfo,
                4
            );


            this->enrollmentGroup->Controls->Add(
                enrollmentLayout
            );


            // =========================================================
            // GUARDIAN GROUP
            // =========================================================

            this->guardianGroup =
                gcnew GroupBox();

            this->guardianGroup->Dock =
                DockStyle::Fill;

            this->guardianGroup->Text =
                L"Guardian Information";

            this->guardianGroup->Padding =
                System::Windows::Forms::Padding(12);

            this->guardianGroup->Margin =
                System::Windows::Forms::Padding(
                    0, 0, 0, 10
                );

            this->guardianGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    FontStyle::Bold
                );


            TableLayoutPanel^ guardianLayout =
                gcnew TableLayoutPanel();

            guardianLayout->Dock =
                DockStyle::Fill;

            guardianLayout->ColumnCount = 4;
            guardianLayout->RowCount = 3;

            guardianLayout->Padding =
                System::Windows::Forms::Padding(5);


            guardianLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    135.0F
                )
            );

            guardianLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    50.0F
                )
            );

            guardianLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    135.0F
                )
            );

            guardianLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    50.0F
                )
            );


            for (int i = 0; i < 3; i++)
            {
                guardianLayout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        40.0F
                    )
                );
            }


            // Guardian Name

            this->lblGuardianName =
                gcnew Label();

            this->lblGuardianName->Text =
                L"Full Name";

            this->lblGuardianName->Dock =
                DockStyle::Fill;

            this->lblGuardianName->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblGuardianName->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);

            this->lblGuardianName->Padding =
                System::Windows::Forms::Padding(0);

            this->lblGuardianName->AutoSize = false;


            this->txtGuardianName =
                gcnew TextBox();

            this->txtGuardianName->Dock =
                DockStyle::Fill;

            this->txtGuardianName->Margin =
                System::Windows::Forms::Padding(3, 6, 3, 6);

            this->txtGuardianName->TextAlign =
                HorizontalAlignment::Left;

            this->txtGuardianName->MinimumSize =
                System::Drawing::Size(
                    0,
                    28
                );


            // Relationship

            this->lblGuardianRelationship =
                gcnew Label();

            this->lblGuardianRelationship->Text =
                L"Relationship";

            this->lblGuardianRelationship->Dock =
                DockStyle::Fill;

            this->lblGuardianRelationship->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblGuardianRelationship->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);

            this->lblGuardianRelationship->Padding =
                System::Windows::Forms::Padding(0);

            this->lblGuardianRelationship->AutoSize = false;


            this->txtGuardianRelationship =
                gcnew TextBox();

            this->txtGuardianRelationship->Dock =
                DockStyle::Fill;

            this->txtGuardianRelationship->Margin =
                System::Windows::Forms::Padding(3, 6, 3, 6);

            this->txtGuardianRelationship->TextAlign =
                HorizontalAlignment::Left;

            this->txtGuardianRelationship->MinimumSize =
                System::Drawing::Size(
                    0,
                    28
                );


            // Phone

            this->lblGuardianPhone =
                gcnew Label();

            this->lblGuardianPhone->Text =
                L"Phone Number";

            this->lblGuardianPhone->Dock =
                DockStyle::Fill;

            this->lblGuardianPhone->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblGuardianPhone->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);

            this->lblGuardianPhone->Padding =
                System::Windows::Forms::Padding(0);

            this->lblGuardianPhone->AutoSize = false;


            this->txtGuardianPhone =
                gcnew TextBox();

            this->txtGuardianPhone->Dock =
                DockStyle::Fill;

            this->txtGuardianPhone->Margin =
                System::Windows::Forms::Padding(3, 6, 3, 6);

            this->txtGuardianPhone->TextAlign =
                HorizontalAlignment::Left;

            this->txtGuardianPhone->MinimumSize =
                System::Drawing::Size(
                    0,
                    28
                );


            // Alternative Phone

            this->lblGuardianAlternativePhone =
                gcnew Label();

            this->lblGuardianAlternativePhone->Text =
                L"Alternative Phone";

            this->lblGuardianAlternativePhone->Dock =
                DockStyle::Fill;

            this->lblGuardianAlternativePhone->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblGuardianAlternativePhone->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);

            this->lblGuardianAlternativePhone->Padding =
                System::Windows::Forms::Padding(0);

            this->lblGuardianAlternativePhone->AutoSize = false;


            this->txtGuardianAlternativePhone =
                gcnew TextBox();

            this->txtGuardianAlternativePhone->Dock =
                DockStyle::Fill;

            this->txtGuardianAlternativePhone->Margin =
                System::Windows::Forms::Padding(3, 6, 3, 6);

            this->txtGuardianAlternativePhone->TextAlign =
                HorizontalAlignment::Left;

            this->txtGuardianAlternativePhone->MinimumSize =
                System::Drawing::Size(
                    0,
                    28
                );


            // Email

            this->lblGuardianEmail =
                gcnew Label();

            this->lblGuardianEmail->Text =
                L"Email (Optional)";

            this->lblGuardianEmail->Dock =
                DockStyle::Fill;

            this->lblGuardianEmail->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblGuardianEmail->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);

            this->lblGuardianEmail->Padding =
                System::Windows::Forms::Padding(0);

            this->lblGuardianEmail->AutoSize = false;


            this->txtGuardianEmail =
                gcnew TextBox();

            this->txtGuardianEmail->Dock =
                DockStyle::Fill;

            this->txtGuardianEmail->Margin =
                System::Windows::Forms::Padding(3, 6, 3, 6);

            this->txtGuardianEmail->TextAlign =
                HorizontalAlignment::Left;

            this->txtGuardianEmail->MinimumSize =
                System::Drawing::Size(
                    0,
                    28
                );


            // Address

            this->lblGuardianAddress =
                gcnew Label();

            this->lblGuardianAddress->Text =
                L"Address";

            this->lblGuardianAddress->Dock =
                DockStyle::Fill;

            this->lblGuardianAddress->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblGuardianAddress->Margin =
                System::Windows::Forms::Padding(3, 0, 3, 0);

            this->lblGuardianAddress->Padding =
                System::Windows::Forms::Padding(0);

            this->lblGuardianAddress->AutoSize = false;


            this->txtGuardianAddress =
                gcnew TextBox();

            this->txtGuardianAddress->Dock =
                DockStyle::Fill;

            this->txtGuardianAddress->Margin =
                System::Windows::Forms::Padding(3, 6, 3, 6);

            this->txtGuardianAddress->TextAlign =
                HorizontalAlignment::Left;

            this->txtGuardianAddress->MinimumSize =
                System::Drawing::Size(
                    0,
                    28
                );


            // ---------------------------------------------------------
            // Add Guardian Controls
            // ---------------------------------------------------------

            guardianLayout->Controls->Add(
                this->lblGuardianName,
                0, 0
            );

            guardianLayout->Controls->Add(
                this->txtGuardianName,
                1, 0
            );

            guardianLayout->Controls->Add(
                this->lblGuardianRelationship,
                2, 0
            );

            guardianLayout->Controls->Add(
                this->txtGuardianRelationship,
                3, 0
            );


            guardianLayout->Controls->Add(
                this->lblGuardianPhone,
                0, 1
            );

            guardianLayout->Controls->Add(
                this->txtGuardianPhone,
                1, 1
            );

            guardianLayout->Controls->Add(
                this->lblGuardianAlternativePhone,
                2, 1
            );

            guardianLayout->Controls->Add(
                this->txtGuardianAlternativePhone,
                3, 1
            );


            guardianLayout->Controls->Add(
                this->lblGuardianEmail,
                0, 2
            );

            guardianLayout->Controls->Add(
                this->txtGuardianEmail,
                1, 2
            );

            guardianLayout->Controls->Add(
                this->lblGuardianAddress,
                2, 2
            );

            guardianLayout->Controls->Add(
                this->txtGuardianAddress,
                3, 2
            );


            this->guardianGroup->Controls->Add(
                guardianLayout
            );


            // =========================================================
            // BUTTON PANEL
            // =========================================================

            this->buttonPanel =
                gcnew Panel();

            this->buttonPanel->Dock =
                DockStyle::Fill;

            this->buttonPanel->Padding =
                System::Windows::Forms::Padding(
                    0,
                    10,
                    0,
                    10
                );

            this->buttonPanel->MinimumSize =
                System::Drawing::Size(
                    0,
                    70
                );


            // Save

            this->btnSave =
                gcnew Button();

            this->btnSave->Text =
                L"Save Student";

            this->btnSave->Width =
                145;

            this->btnSave->Height =
                42;

            this->btnSave->Dock =
                DockStyle::Right;

            this->btnSave->BackColor =
                Color::FromArgb(38, 117, 92);

            this->btnSave->ForeColor =
                Color::White;

            this->btnSave->FlatStyle =
                FlatStyle::Flat;


            // Back

            this->btnCancel =
                gcnew Button();

            this->btnCancel->Text =
                L"Back to Students";

            this->btnCancel->Width =
                145;

            this->btnCancel->Height =
                42;


            // Clear

            this->btnClear =
                gcnew Button();

            this->btnClear->Text =
                L"Clear";

            this->btnClear->Width =
                105;

            this->btnClear->Height =
                42;


            // Put buttons next to each other

            this->btnCancel->Location =
                Point(0, 12);

            this->btnClear->Location =
                Point(155, 12);


            this->buttonPanel->Controls->Add(
                this->btnSave
            );

            this->buttonPanel->Controls->Add(
                this->btnClear
            );

            this->buttonPanel->Controls->Add(
                this->btnCancel
            );


            // =========================================================
            // MAIN LAYOUT CONTROLS
            // =========================================================

            this->mainLayout->Controls->Add(
                this->headerPanel,
                0, 0
            );

            this->mainLayout->Controls->Add(
                this->studentGroup,
                0, 1
            );

            this->mainLayout->Controls->Add(
                this->enrollmentGroup,
                0, 2
            );

            this->mainLayout->Controls->Add(
                this->guardianGroup,
                0, 3
            );

            this->mainLayout->Controls->Add(
                this->buttonPanel,
                0, 4
            );


            // =========================================================
            // PUT MAIN LAYOUT INSIDE SCROLL PANEL
            // =========================================================

            // Keep a little extra room after the action buttons so the
            // entire bottom row can be reached comfortably by scrolling.
            this->mainLayout->Margin =
                System::Windows::Forms::Padding(
                    0,
                    0,
                    0,
                    20
                );

            this->mainLayout->MinimumSize =
                System::Drawing::Size(
                    0,
                    900
                );

            this->scrollPanel->Controls->Add(
                this->mainLayout
            );


            // =========================================================
            // PUT SCROLL PANEL ON FORM
            // =========================================================

            this->Controls->Add(
                this->scrollPanel
            );


            this->ResumeLayout(false);
        }


        // =========================================================
        // LOAD ACADEMIC YEARS
        // =========================================================

        void LoadAcademicYears()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT academic_year_id, year_name "
                        "FROM academic_years "
                        "WHERE status = 'Active' "
                        "ORDER BY academic_year_id DESC"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                this->cmbAcademicYear->Items->Clear();

                this->cmbAcademicYear->Items->Add(
                    L"Select Academic Year"
                );

                while (result->next())
                {
                    this->cmbAcademicYear->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "academic_year_id"
                            ),
                            gcnew String(
                                result->getString(
                                    "year_name"
                                ).c_str()
                            )
                        )
                    );
                }

                this->cmbAcademicYear->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // LOAD TERMS
        // =========================================================

        void LoadTerms()
        {
            this->cmbTerm->Items->Clear();

            this->cmbTerm->Items->Add(
                L"Select Term"
            );

            this->cmbTerm->SelectedIndex = 0;
            this->cmbTerm->Enabled = false;


            this->cmbStream->Items->Clear();

            this->cmbStream->Items->Add(
                L"Select Stream"
            );

            this->cmbStream->SelectedIndex = 0;
            this->cmbStream->Enabled = false;


            this->lblStreamInfo->Text =
                L"Select an academic year, term and class "
                L"to view stream enrollment.";


            if (this->cmbAcademicYear->SelectedIndex <= 0)
            {
                return;
            }


            ComboItem^ yearItem =
                safe_cast<ComboItem^>(
                    this->cmbAcademicYear->SelectedItem
                    );


            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT term_id, term_name "
                        "FROM terms "
                        "WHERE academic_year_id = ? "
                        "AND status = 'Active' "
                        "ORDER BY term_id"
                    )
                );

                stmt->setInt(
                    1,
                    yearItem->Id
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );


                while (result->next())
                {
                    this->cmbTerm->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "term_id"
                            ),
                            gcnew String(
                                result->getString(
                                    "term_name"
                                ).c_str()
                            )
                        )
                    
                    );
                }


                if (this->cmbTerm->Items->Count > 1)
                {
                    this->cmbTerm->Enabled = true;
                }
                else
                {
                    this->cmbTerm->Enabled = false;
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // LOAD CLASSES
        // =========================================================

        void LoadClasses()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT class_id, class_name "
                        "FROM classes "
                        "WHERE status = 'Active' "
                        "ORDER BY class_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );


                this->cmbClass->Items->Clear();

                this->cmbClass->Items->Add(
                    L"Select Class"
                );


                while (result->next())
                {
                    this->cmbClass->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "class_id"
                            ),
                            gcnew String(
                                result->getString(
                                    "class_name"
                                ).c_str()
                            )
                        )
                    );
                }


                this->cmbClass->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // LOAD STREAMS
        // =========================================================

        void LoadStreams()
        {
            this->cmbStream->Items->Clear();

            this->cmbStream->Items->Add(
                L"Select Stream"
            );

            this->cmbStream->SelectedIndex = 0;
            this->cmbStream->Enabled = false;


            this->lblStreamInfo->Text =
                L"Select an academic year, term and class "
                L"to view stream enrollment.";


            if (
                this->cmbAcademicYear->SelectedIndex <= 0 ||
                this->cmbTerm->SelectedIndex <= 0 ||
                this->cmbClass->SelectedIndex <= 0
                )
            {
                return;
            }


            ComboItem^ yearItem =
                safe_cast<ComboItem^>(
                    this->cmbAcademicYear->SelectedItem
                    );

            ComboItem^ termItem =
                safe_cast<ComboItem^>(
                    this->cmbTerm->SelectedItem
                    );

            ComboItem^ classItem =
                safe_cast<ComboItem^>(
                    this->cmbClass->SelectedItem
                    );


            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "s.stream_id, "
                        "s.stream_name, "
                        "COUNT(e.enrollment_id) AS student_count "
                        "FROM streams s "
                        "LEFT JOIN enrollments e "
                        "ON e.stream_id = s.stream_id "
                        "AND e.academic_year_id = ? "
                        "AND e.term_id = ? "
                        "AND e.class_id = ? "
                        "AND e.status = 'Active' "
                        "WHERE s.class_id = ? "
                        "AND s.status = 'Active' "
                        "GROUP BY "
                        "s.stream_id, "
                        "s.stream_name "
                        "ORDER BY s.stream_name"
                    )
                );


                stmt->setInt(
                    1,
                    yearItem->Id
                );

                stmt->setInt(
                    2,
                    termItem->Id
                );

                stmt->setInt(
                    3,
                    classItem->Id
                );

                stmt->setInt(
                    4,
                    classItem->Id
                );


                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );


                while (result->next())
                {
                    this->cmbStream->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "stream_id"
                            ),
                            gcnew String(
                                result->getString(
                                    "stream_name"
                                ).c_str()
                            ),
                            result->getInt(
                                "student_count"
                            )
                        )
                    );
                }


                if (this->cmbStream->Items->Count > 1)
                {
                    this->cmbStream->Enabled = true;
                }
                else
                {
                    this->cmbStream->Enabled = false;
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // ACADEMIC YEAR CHANGED
        // =========================================================

        System::Void cmbAcademicYear_SelectedIndexChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            LoadTerms();
            LoadStreams();
        }


        // =========================================================
        // TERM CHANGED
        // =========================================================

        System::Void cmbTerm_SelectedIndexChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            LoadStreams();
        }


        // =========================================================
        // CLASS CHANGED
        // =========================================================

        System::Void cmbClass_SelectedIndexChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            LoadStreams();
        }


        // =========================================================
        // STREAM CHANGED
        // =========================================================

        System::Void cmbStream_SelectedIndexChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (this->cmbStream->SelectedIndex <= 0)
            {
                this->lblStreamInfo->Text =
                    L"Select an academic year, term and class "
                    L"to view stream enrollment.";

                return;
            }


            ComboItem^ streamItem =
                safe_cast<ComboItem^>(
                    this->cmbStream->SelectedItem
                    );


            this->lblStreamInfo->Text =
                L"Current enrollment in " +
                streamItem->Text +
                L": " +
                streamItem->Count.ToString() +
                L" students.";
        }


        // =========================================================
        // SELECT COMBO ITEM BY ID
        // =========================================================

        bool SelectComboItemById(
            ComboBox^ combo,
            int id)
        {
            for (int i = 0; i < combo->Items->Count; i++)
            {
                ComboItem^ item =
                    dynamic_cast<ComboItem^>(
                        combo->Items[i]
                    );

                if (item != nullptr && item->Id == id)
                {
                    combo->SelectedIndex = i;
                    return true;
                }
            }

            return false;
        }


        // =========================================================
        // LOAD STUDENT FOR EDIT
        // =========================================================

        void LoadStudentForEdit(
            long long studentId)
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "s.registration_number, "
                        "s.first_name, "
                        "s.middle_name, "
                        "s.last_name, "
                        "s.date_of_birth, "
                        "s.gender, "
                        "s.admission_date, "
                        "s.home_address, "
                        "s.photo_path, "
                        "s.status, "
                        "e.enrollment_id, "
                        "e.academic_year_id, "
                        "e.term_id, "
                        "e.class_id, "
                        "e.stream_id, "
                        "g.guardian_id, "
                        "g.full_name AS guardian_name, "
                        "g.relationship AS guardian_relationship, "
                        "g.phone_number AS guardian_phone, "
                        "g.alternative_phone AS guardian_alternative_phone, "
                        "g.email AS guardian_email, "
                        "g.address AS guardian_address "
                        "FROM students s "
                        "LEFT JOIN enrollments e "
                        "ON e.enrollment_id = ("
                            "SELECT e2.enrollment_id "
                            "FROM enrollments e2 "
                            "WHERE e2.student_id = s.student_id "
                            "AND e2.status = 'Active' "
                            "ORDER BY e2.enrollment_date DESC, e2.enrollment_id DESC "
                            "LIMIT 1"
                        ") "
                        "LEFT JOIN student_guardians sg "
                        "ON sg.student_id = s.student_id "
                        "AND sg.is_primary = 1 "
                        "LEFT JOIN guardians g "
                        "ON g.guardian_id = sg.guardian_id "
                        "WHERE s.student_id = ? "
                        "LIMIT 1"
                    )
                );

                stmt->setInt64(
                    1,
                    studentId
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                if (!result->next())
                {
                    MessageBox::Show(
                        L"Student record could not be found.",
                        L"Edit Student",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    this->Close();
                    return;
                }

                this->editingPhotoPath =
                    result->isNull("photo_path")
                    ? L""
                    : gcnew String(
                        result->getString(
                            "photo_path"
                        ).c_str()
                    );

                this->selectedPhotoSourcePath =
                    nullptr;

                this->editingRegistrationNumber =
                    gcnew String(
                        result->getString(
                            "registration_number"
                        ).c_str()
                    );

                if (
                    !String::IsNullOrWhiteSpace(
                        this->editingPhotoPath
                    )
                )
                {
                    ShowPhotoPreview(
                        System::IO::Path::Combine(
                            Application::StartupPath,
                            this->editingPhotoPath
                        )
                    );
                }
                else
                {
                    ShowPhotoPreview(
                        L""
                    );
                }

                this->editingGuardianId =
                    result->isNull("guardian_id")
                    ? 0
                    : result->getInt64("guardian_id");

                this->editingEnrollmentId =
                    result->isNull("enrollment_id")
                    ? 0
                    : result->getInt("enrollment_id");

                this->txtFirstName->Text =
                    gcnew String(
                        result->getString("first_name").c_str()
                    );

                if (!result->isNull("middle_name"))
                {
                    this->txtMiddleName->Text =
                        gcnew String(
                            result->getString("middle_name").c_str()
                        );
                }

                this->txtLastName->Text =
                    gcnew String(
                        result->getString("last_name").c_str()
                    );

                this->dtpDob->Value =
                    DateTime::ParseExact(
                        gcnew String(
                            result->getString("date_of_birth").c_str()
                        ),
                        L"yyyy-MM-dd",
                        System::Globalization::CultureInfo::InvariantCulture
                    );

                String^ gender =
                    gcnew String(
                        result->getString("gender").c_str()
                    );

                for (int i = 0; i < this->cmbGender->Items->Count; i++)
                {
                    if (
                        Convert::ToString(
                            this->cmbGender->Items[i]
                        )->Equals(
                            gender,
                            StringComparison::OrdinalIgnoreCase
                        )
                    )
                    {
                        this->cmbGender->SelectedIndex = i;
                        break;
                    }
                }

                this->dtpAdmissionDate->Value =
                    DateTime::ParseExact(
                        gcnew String(
                            result->getString("admission_date").c_str()
                        ),
                        L"yyyy-MM-dd",
                        System::Globalization::CultureInfo::InvariantCulture
                    );

                this->txtHomeAddress->Text =
                    gcnew String(
                        result->getString("home_address").c_str()
                    );

                if (!result->isNull("guardian_name"))
                {
                    this->txtGuardianName->Text =
                        gcnew String(
                            result->getString("guardian_name").c_str()
                        );
                }

                if (!result->isNull("guardian_relationship"))
                {
                    this->txtGuardianRelationship->Text =
                        gcnew String(
                            result->getString("guardian_relationship").c_str()
                        );
                }

                if (!result->isNull("guardian_phone"))
                {
                    this->txtGuardianPhone->Text =
                        gcnew String(
                            result->getString("guardian_phone").c_str()
                        );
                }

                if (!result->isNull("guardian_alternative_phone"))
                {
                    this->txtGuardianAlternativePhone->Text =
                        gcnew String(
                            result->getString("guardian_alternative_phone").c_str()
                        );
                }

                if (!result->isNull("guardian_email"))
                {
                    this->txtGuardianEmail->Text =
                        gcnew String(
                            result->getString("guardian_email").c_str()
                        );
                }

                if (!result->isNull("guardian_address"))
                {
                    this->txtGuardianAddress->Text =
                        gcnew String(
                            result->getString("guardian_address").c_str()
                        );
                }

                int classId =
                    result->isNull("class_id")
                    ? 0
                    : result->getInt("class_id");

                int academicYearId =
                    result->isNull("academic_year_id")
                    ? 0
                    : result->getInt("academic_year_id");

                int termId =
                    result->isNull("term_id")
                    ? 0
                    : result->getInt("term_id");

                int streamId =
                    result->isNull("stream_id")
                    ? 0
                    : result->getInt("stream_id");

                // Select the class first.
                if (classId > 0)
                {
                    SelectComboItemById(
                        this->cmbClass,
                        classId
                    );
                }

                // Selecting the year loads its terms.
                if (academicYearId > 0)
                {
                    SelectComboItemById(
                        this->cmbAcademicYear,
                        academicYearId
                    );
                }

                // Selecting the term loads the streams for the
                // already-selected class.
                if (termId > 0)
                {
                    SelectComboItemById(
                        this->cmbTerm,
                        termId
                    );
                }

                if (streamId > 0)
                {
                    SelectComboItemById(
                        this->cmbStream,
                        streamId
                    );
                }

                String^ registration =
                    gcnew String(
                        result->getString("registration_number").c_str()
                    );

                this->lblSubtitle->Text =
                    L"Editing student " +
                    registration +
                    L". Registration number cannot be changed.";

                this->btnSave->Text =
                    L"Update Student";

                this->btnClear->Enabled =
                    false;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (System::Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // UPDATE EXISTING STUDENT
        // =========================================================

        void UpdateStudent()
        {
            ComboItem^ yearItem =
                safe_cast<ComboItem^>(
                    this->cmbAcademicYear->SelectedItem
                );

            ComboItem^ termItem =
                safe_cast<ComboItem^>(
                    this->cmbTerm->SelectedItem
                );

            ComboItem^ classItem =
                safe_cast<ComboItem^>(
                    this->cmbClass->SelectedItem
                );

            ComboItem^ streamItem =
                safe_cast<ComboItem^>(
                    this->cmbStream->SelectedItem
                );

            std::unique_ptr<sql::Connection> con;

            try
            {
                con =
                    DbConnection::GetConnection();

                con->setAutoCommit(false);

                std::unique_ptr<sql::PreparedStatement>
                    studentStmt(
                        con->prepareStatement(
                            "UPDATE students "
                            "SET first_name = ?, "
                            "middle_name = ?, "
                            "last_name = ?, "
                            "date_of_birth = ?, "
                            "gender = ?, "
                            "admission_date = ?, "
                            "home_address = ?, "
                            "photo_path = ? "
                            "WHERE student_id = ?"
                        )
                    );

                studentStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        this->txtFirstName->Text->Trim()
                    )
                );

                studentStmt->setString(
                    2,
                    msclr::interop::marshal_as<std::string>(
                        this->txtMiddleName->Text->Trim()
                    )
                );

                studentStmt->setString(
                    3,
                    msclr::interop::marshal_as<std::string>(
                        this->txtLastName->Text->Trim()
                    )
                );

                studentStmt->setString(
                    4,
                    msclr::interop::marshal_as<std::string>(
                        this->dtpDob->Value.Date.ToString("yyyy-MM-dd")
                    )
                );

                studentStmt->setString(
                    5,
                    msclr::interop::marshal_as<std::string>(
                        this->cmbGender->SelectedItem->ToString()
                    )
                );

                studentStmt->setString(
                    6,
                    msclr::interop::marshal_as<std::string>(
                        this->dtpAdmissionDate->Value.Date.ToString("yyyy-MM-dd")
                    )
                );

                studentStmt->setString(
                    7,
                    msclr::interop::marshal_as<std::string>(
                        this->txtHomeAddress->Text->Trim()
                    )
                );

                String^ storedPhotoPath =
                    this->editingPhotoPath;

                if (
                    !String::IsNullOrWhiteSpace(
                        this->selectedPhotoSourcePath
                    )
                )
                {
                    storedPhotoPath =
                        SaveStudentPhoto(
                            this->selectedPhotoSourcePath,
                            this->editingRegistrationNumber
                        );
                }

                studentStmt->setString(
                    8,
                    msclr::interop::marshal_as<std::string>(
                        storedPhotoPath
                    )
                );

                studentStmt->setInt64(
                    9,
                    this->editingStudentId
                );

                studentStmt->executeUpdate();


                if (this->editingGuardianId > 0)
                {
                    std::unique_ptr<sql::PreparedStatement>
                        guardianStmt(
                            con->prepareStatement(
                                "UPDATE guardians "
                                "SET full_name = ?, "
                                "relationship = ?, "
                                "phone_number = ?, "
                                "alternative_phone = ?, "
                                "email = ?, "
                                "address = ? "
                                "WHERE guardian_id = ?"
                            )
                        );

                    guardianStmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianName->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        2,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianRelationship->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        3,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianPhone->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        4,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianAlternativePhone->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        5,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianEmail->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        6,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianAddress->Text->Trim()
                        )
                    );

                    guardianStmt->setInt64(
                        7,
                        this->editingGuardianId
                    );

                    guardianStmt->executeUpdate();
                }
                else
                {
                    std::unique_ptr<sql::PreparedStatement>
                        guardianStmt(
                            con->prepareStatement(
                                "INSERT INTO guardians "
                                "(full_name, relationship, phone_number, alternative_phone, email, address) "
                                "VALUES (?, ?, ?, ?, ?, ?)"
                            )
                        );

                    guardianStmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianName->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        2,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianRelationship->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        3,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianPhone->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        4,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianAlternativePhone->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        5,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianEmail->Text->Trim()
                        )
                    );

                    guardianStmt->setString(
                        6,
                        msclr::interop::marshal_as<std::string>(
                            this->txtGuardianAddress->Text->Trim()
                        )
                    );

                    guardianStmt->executeUpdate();

                    std::unique_ptr<sql::PreparedStatement>
                        guardianIdStmt(
                            con->prepareStatement(
                                "SELECT LAST_INSERT_ID() AS guardian_id"
                            )
                        );

                    std::unique_ptr<sql::ResultSet>
                        guardianIdResult(
                            guardianIdStmt->executeQuery()
                        );

                    if (!guardianIdResult->next())
                    {
                        throw std::runtime_error(
                            "Unable to retrieve the guardian ID."
                        );
                    }

                    this->editingGuardianId =
                        guardianIdResult->getInt64(
                            "guardian_id"
                        );

                    std::unique_ptr<sql::PreparedStatement>
                        linkStmt(
                            con->prepareStatement(
                                "INSERT INTO student_guardians "
                                "(student_id, guardian_id, is_primary) "
                                "VALUES (?, ?, 1)"
                            )
                        );

                    linkStmt->setInt64(
                        1,
                        this->editingStudentId
                    );

                    linkStmt->setInt64(
                        2,
                        this->editingGuardianId
                    );

                    linkStmt->executeUpdate();
                }


                if (this->editingEnrollmentId > 0)
                {
                    std::unique_ptr<sql::PreparedStatement>
                        enrollmentStmt(
                            con->prepareStatement(
                                "UPDATE enrollments "
                                "SET academic_year_id = ?, "
                                "term_id = ?, "
                                "class_id = ?, "
                                "stream_id = ?, "
                                "enrollment_date = ?, "
                                "status = 'Active' "
                                "WHERE enrollment_id = ? "
                                "AND student_id = ?"
                            )
                        );

                    enrollmentStmt->setInt(
                        1,
                        yearItem->Id
                    );

                    enrollmentStmt->setInt(
                        2,
                        termItem->Id
                    );

                    enrollmentStmt->setInt(
                        3,
                        classItem->Id
                    );

                    enrollmentStmt->setInt(
                        4,
                        streamItem->Id
                    );

                    enrollmentStmt->setString(
                        5,
                        msclr::interop::marshal_as<std::string>(
                            this->dtpAdmissionDate->Value.Date.ToString("yyyy-MM-dd")
                        )
                    );

                    enrollmentStmt->setInt(
                        6,
                        this->editingEnrollmentId
                    );

                    enrollmentStmt->setInt64(
                        7,
                        this->editingStudentId
                    );

                    enrollmentStmt->executeUpdate();
                }
                else
                {
                    std::unique_ptr<sql::PreparedStatement>
                        enrollmentStmt(
                            con->prepareStatement(
                                "INSERT INTO enrollments "
                                "(student_id, academic_year_id, term_id, class_id, stream_id, enrollment_date, status) "
                                "VALUES (?, ?, ?, ?, ?, ?, 'Active')"
                            )
                        );

                    enrollmentStmt->setInt64(
                        1,
                        this->editingStudentId
                    );

                    enrollmentStmt->setInt(
                        2,
                        yearItem->Id
                    );

                    enrollmentStmt->setInt(
                        3,
                        termItem->Id
                    );

                    enrollmentStmt->setInt(
                        4,
                        classItem->Id
                    );

                    enrollmentStmt->setInt(
                        5,
                        streamItem->Id
                    );

                    enrollmentStmt->setString(
                        6,
                        msclr::interop::marshal_as<std::string>(
                            this->dtpAdmissionDate->Value.Date.ToString("yyyy-MM-dd")
                        )
                    );

                    enrollmentStmt->executeUpdate();
                }

                con->commit();

                MessageBox::Show(
                    L"Student details updated successfully.",
                    L"Edit Student",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );

                this->DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                this->Close();
            }
            catch (sql::SQLException& ex)
            {
                if (con)
                {
                    try
                    {
                        con->rollback();
                    }
                    catch (...)
                    {
                    }
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (std::exception& ex)
            {
                if (con)
                {
                    try
                    {
                        con->rollback();
                    }
                    catch (...)
                    {
                    }
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // SAVE STUDENT
        // =========================================================

        System::Void btnSave_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            // -----------------------------------------------------
            // BASIC VALIDATION
            // -----------------------------------------------------

            if (String::IsNullOrWhiteSpace(
                this->txtFirstName->Text))
            {
                MessageBox::Show(
                    L"Please enter the student's first name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtFirstName->Focus();
                return;
            }


            if (String::IsNullOrWhiteSpace(
                this->txtLastName->Text))
            {
                MessageBox::Show(
                    L"Please enter the student's last name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtLastName->Focus();
                return;
            }


            DateTime dob = this->dtpDob->Value.Date;

            if (dob >= DateTime::Today)
            {
                MessageBox::Show(
                    L"Date of birth must be before today.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->dtpDob->Focus();
                return;
            }


            if (this->cmbGender->SelectedIndex <= 0)
            {
                MessageBox::Show(
                    L"Please select the student's gender.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->cmbGender->Focus();
                return;
            }


            DateTime admissionDate =
                this->dtpAdmissionDate->Value.Date;


            if (admissionDate < dob)
            {
                MessageBox::Show(
                    L"Admission date cannot be before the date of birth.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->dtpAdmissionDate->Focus();
                return;
            }


            if (String::IsNullOrWhiteSpace(
                this->txtHomeAddress->Text))
            {
                MessageBox::Show(
                    L"Please enter the student's home address.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtHomeAddress->Focus();
                return;
            }


            // -----------------------------------------------------
            // GUARDIAN VALIDATION
            // -----------------------------------------------------

            if (String::IsNullOrWhiteSpace(
                this->txtGuardianName->Text))
            {
                MessageBox::Show(
                    L"Please enter the guardian's full name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtGuardianName->Focus();
                return;
            }


            if (String::IsNullOrWhiteSpace(
                this->txtGuardianRelationship->Text))
            {
                MessageBox::Show(
                    L"Please enter the guardian's relationship "
                    L"to the student.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtGuardianRelationship->Focus();
                return;
            }


            if (String::IsNullOrWhiteSpace(
                this->txtGuardianPhone->Text))
            {
                MessageBox::Show(
                    L"Please enter the guardian's phone number.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtGuardianPhone->Focus();
                return;
            }


            // -----------------------------------------------------
            // ENROLLMENT VALIDATION
            // -----------------------------------------------------

            if (this->cmbAcademicYear->SelectedIndex <= 0)
            {
                MessageBox::Show(
                    L"Please select an academic year.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->cmbAcademicYear->Focus();
                return;
            }


            if (this->cmbTerm->SelectedIndex <= 0)
            {
                MessageBox::Show(
                    L"Please select a term.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->cmbTerm->Focus();
                return;
            }


            if (this->cmbClass->SelectedIndex <= 0)
            {
                MessageBox::Show(
                    L"Please select a class.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->cmbClass->Focus();
                return;
            }


            if (this->cmbStream->SelectedIndex <= 0)
            {
                MessageBox::Show(
                    L"Please select a stream.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->cmbStream->Focus();
                return;
            }


            // -----------------------------------------------------
            // SELECTED ENROLLMENT ITEMS
            // -----------------------------------------------------

            ComboItem^ yearItem =
                safe_cast<ComboItem^>(
                    this->cmbAcademicYear->SelectedItem
                    );

            ComboItem^ termItem =
                safe_cast<ComboItem^>(
                    this->cmbTerm->SelectedItem
                    );

            ComboItem^ classItem =
                safe_cast<ComboItem^>(
                    this->cmbClass->SelectedItem
                    );

            ComboItem^ streamItem =
                safe_cast<ComboItem^>(
                    this->cmbStream->SelectedItem
                    );


            if (this->editMode)
            {
                UpdateStudent();
                return;
            }


            // -----------------------------------------------------
            // DATABASE TRANSACTION
            // -----------------------------------------------------

            std::unique_ptr<sql::Connection> con;

            try
            {
                con = DbConnection::GetConnection();

                con->setAutoCommit(false);


                // =================================================
                // 1. GENERATE REGISTRATION NUMBER
                // =================================================

                int registrationYear =
                    admissionDate.Year;


                std::unique_ptr<sql::PreparedStatement>
                    sequenceStmt(
                        con->prepareStatement(
                            "INSERT INTO "
                            "student_registration_sequences "
                            "(registration_year, last_sequence) "
                            "VALUES (?, 1) "
                            "ON DUPLICATE KEY UPDATE "
                            "last_sequence = "
                            "last_sequence + 1"
                        )
                    );


                sequenceStmt->setInt(
                    1,
                    registrationYear
                );

                sequenceStmt->executeUpdate();


                std::unique_ptr<sql::PreparedStatement>
                    sequenceSelect(
                        con->prepareStatement(
                            "SELECT last_sequence "
                            "FROM student_registration_sequences "
                            "WHERE registration_year = ? "
                            "FOR UPDATE"
                        )
                    );


                sequenceSelect->setInt(
                    1,
                    registrationYear
                );


                std::unique_ptr<sql::ResultSet>
                    sequenceResult(
                        sequenceSelect->executeQuery()
                    );


                if (!sequenceResult->next())
                {
                    throw std::runtime_error(
                        "Unable to generate student registration number."
                    );
                }


                int sequenceNumber =
                    sequenceResult->getInt(
                        "last_sequence"
                    );


                std::ostringstream registrationStream;

                registrationStream
                    << "STU/"
                    << registrationYear
                    << "/"
                    << std::setw(4)
                    << std::setfill('0')
                    << sequenceNumber;


                std::string registrationNumber =
                    registrationStream.str();


                // =================================================
                // 2. INSERT STUDENT
                // =================================================

                std::unique_ptr<sql::PreparedStatement>
                    studentStmt(
                        con->prepareStatement(
                            "INSERT INTO students "
                            "("
                            "registration_number, "
                            "registration_year, "
                            "registration_sequence, "
                            "photo_path, "
                            "first_name, "
                            "middle_name, "
                            "last_name, "
                            "date_of_birth, "
                            "gender, "
                            "admission_date, "
                            "home_address, "
                            "status"
                            ") "
                            "VALUES "
                            "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
                        )
                    );


                studentStmt->setString(
                    1,
                    registrationNumber
                );

                studentStmt->setInt(
                    2,
                    registrationYear
                );

                studentStmt->setInt(
                    3,
                    sequenceNumber
                );

                String^ storedPhotoPath =
                    L"";

                if (
                    !String::IsNullOrWhiteSpace(
                        this->selectedPhotoSourcePath
                    )
                )
                {
                    storedPhotoPath =
                        SaveStudentPhoto(
                            this->selectedPhotoSourcePath,
                            gcnew String(
                                registrationNumber.c_str()
                            )
                        );
                }

                studentStmt->setString(
                    4,
                    msclr::interop::marshal_as<std::string>(
                        storedPhotoPath
                    )
                );

                studentStmt->setString(
                    5,
                    msclr::interop::marshal_as<std::string>(
                        this->txtFirstName->Text->Trim()
                    )
                );

                studentStmt->setString(
                    6,
                    msclr::interop::marshal_as<std::string>(
                        this->txtMiddleName->Text->Trim()
                    )
                );

                studentStmt->setString(
                    7,
                    msclr::interop::marshal_as<std::string>(
                        this->txtLastName->Text->Trim()
                    )
                );

                studentStmt->setString(
                    8,
                    msclr::interop::marshal_as<std::string>(
                        dob.ToString("yyyy-MM-dd")
                    )
                );

                studentStmt->setString(
                    9,
                    msclr::interop::marshal_as<std::string>(
                        this->cmbGender->SelectedItem->ToString()
                    )
                );

                studentStmt->setString(
                    10,
                    msclr::interop::marshal_as<std::string>(
                        admissionDate.ToString("yyyy-MM-dd")
                    )
                );

                studentStmt->setString(
                    11,
                    msclr::interop::marshal_as<std::string>(
                        this->txtHomeAddress->Text->Trim()
                    )
                );

                studentStmt->setString(
                    12,
                    "Active"
                );


                studentStmt->executeUpdate();


                // =================================================
                // 3. GET STUDENT ID
                // =================================================

                std::unique_ptr<sql::PreparedStatement>
                    studentIdStmt(
                        con->prepareStatement(
                            "SELECT LAST_INSERT_ID() AS student_id"
                        )
                    );


                std::unique_ptr<sql::ResultSet>
                    studentIdResult(
                        studentIdStmt->executeQuery()
                    );


                if (!studentIdResult->next())
                {
                    throw std::runtime_error(
                        "Unable to retrieve the new student ID."
                    );
                }


                long long studentId =
                    studentIdResult->getInt64(
                        "student_id"
                    );


                // =================================================
                // 4. INSERT GUARDIAN
                // =================================================

                std::unique_ptr<sql::PreparedStatement>
                    guardianStmt(
                        con->prepareStatement(
                            "INSERT INTO guardians "
                            "("
                            "full_name, "
                            "relationship, "
                            "phone_number, "
                            "alternative_phone, "
                            "email, "
                            "address"
                            ") "
                            "VALUES (?, ?, ?, ?, ?, ?)"
                        )
                    );


                guardianStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        this->txtGuardianName->Text->Trim()
                    )
                );

                guardianStmt->setString(
                    2,
                    msclr::interop::marshal_as<std::string>(
                        this->txtGuardianRelationship->Text->Trim()
                    )
                );

                guardianStmt->setString(
                    3,
                    msclr::interop::marshal_as<std::string>(
                        this->txtGuardianPhone->Text->Trim()
                    )
                );

                guardianStmt->setString(
                    4,
                    msclr::interop::marshal_as<std::string>(
                        this->txtGuardianAlternativePhone->Text->Trim()
                    )
                );

                guardianStmt->setString(
                    5,
                    msclr::interop::marshal_as<std::string>(
                        this->txtGuardianEmail->Text->Trim()
                    )
                );

                guardianStmt->setString(
                    6,
                    msclr::interop::marshal_as<std::string>(
                        this->txtGuardianAddress->Text->Trim()
                    )
                );


                guardianStmt->executeUpdate();


                // =================================================
                // 5. GET GUARDIAN ID
                // =================================================

                std::unique_ptr<sql::PreparedStatement>
                    guardianIdStmt(
                        con->prepareStatement(
                            "SELECT LAST_INSERT_ID() AS guardian_id"
                        )
                    );


                std::unique_ptr<sql::ResultSet>
                    guardianIdResult(
                        guardianIdStmt->executeQuery()
                    );


                if (!guardianIdResult->next())
                {
                    throw std::runtime_error(
                        "Unable to retrieve the guardian ID."
                    );
                }


                long long guardianId =
                    guardianIdResult->getInt64(
                        "guardian_id"
                    );


                // =================================================
                // 6. LINK STUDENT TO GUARDIAN
                // =================================================

                std::unique_ptr<sql::PreparedStatement>
                    linkStmt(
                        con->prepareStatement(
                            "INSERT INTO student_guardians "
                            "(student_id, guardian_id, is_primary) "
                            "VALUES (?, ?, ?)"
                        )
                    );


                linkStmt->setInt64(
                    1,
                    studentId
                );

                linkStmt->setInt64(
                    2,
                    guardianId
                );

                linkStmt->setBoolean(
                    3,
                    true
                );


                linkStmt->executeUpdate();


                // =================================================
                // 7. INSERT ENROLLMENT
                // =================================================

                std::unique_ptr<sql::PreparedStatement>
                    enrollmentStmt(
                        con->prepareStatement(
                            "INSERT INTO enrollments "
                            "("
                            "student_id, "
                            "academic_year_id, "
                            "term_id, "
                            "class_id, "
                            "stream_id, "
                            "enrollment_date, "
                            "status"
                            ") "
                            "VALUES (?, ?, ?, ?, ?, ?, ?)"
                        )
                    );


                enrollmentStmt->setInt64(
                    1,
                    studentId
                );

                enrollmentStmt->setInt(
                    2,
                    yearItem->Id
                );

                enrollmentStmt->setInt(
                    3,
                    termItem->Id
                );

                enrollmentStmt->setInt(
                    4,
                    classItem->Id
                );

                enrollmentStmt->setInt(
                    5,
                    streamItem->Id
                );

                enrollmentStmt->setString(
                    6,
                    msclr::interop::marshal_as<std::string>(
                        admissionDate.ToString("yyyy-MM-dd")
                    )
                );

                enrollmentStmt->setString(
                    7,
                    "Active"
                );


                enrollmentStmt->executeUpdate();


                // =================================================
                // 8. COMMIT
                // =================================================

                con->commit();


                MessageBox::Show(
                    L"Student registered successfully.\n\n"
                    L"Registration Number: " +
                    gcnew String(registrationNumber.c_str()),
                    L"Student Registration",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );


                this->DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                this->Close();
            }
            catch (sql::SQLException& ex)
            {
                if (con)
                {
                    try
                    {
                        con->rollback();
                    }
                    catch (...)
                    {
                    }
                }


                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (std::exception& ex)
            {
                if (con)
                {
                    try
                    {
                        con->rollback();
                    }
                    catch (...)
                    {
                    }
                }


                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // CHOOSE STUDENT PHOTO
        // =========================================================

        System::Void btnChoosePhoto_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            OpenFileDialog^ dialog =
                gcnew OpenFileDialog();

            dialog->Title =
                L"Select Student Photo";

            dialog->Filter =
                L"Image Files|*.jpg;*.jpeg;*.png;*.bmp";

            dialog->Multiselect =
                false;

            if (
                dialog->ShowDialog(this)
                ==
                System::Windows::Forms::DialogResult::OK
            )
            {
                this->selectedPhotoSourcePath =
                    dialog->FileName;

                ShowPhotoPreview(
                    dialog->FileName
                );
            }
        }


        // =========================================================
        // CLEAR FORM
        // =========================================================

        System::Void btnClear_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->txtFirstName->Clear();
            this->txtMiddleName->Clear();
            this->txtLastName->Clear();

            this->cmbGender->SelectedIndex = 0;

            this->dtpDob->Value =
                DateTime::Today.AddYears(-15);

            this->dtpAdmissionDate->Value =
                DateTime::Today;

            this->txtHomeAddress->Clear();


            this->txtGuardianName->Clear();
            this->txtGuardianRelationship->Clear();
            this->txtGuardianPhone->Clear();
            this->txtGuardianAlternativePhone->Clear();
            this->txtGuardianEmail->Clear();
            this->txtGuardianAddress->Clear();


            this->cmbAcademicYear->SelectedIndex = 0;
            this->cmbClass->SelectedIndex = 0;

            this->cmbTerm->Items->Clear();
            this->cmbTerm->Items->Add(
                L"Select Term"
            );
            this->cmbTerm->SelectedIndex = 0;
            this->cmbTerm->Enabled = false;


            this->cmbStream->Items->Clear();
            this->cmbStream->Items->Add(
                L"Select Stream"
            );
            this->cmbStream->SelectedIndex = 0;
            this->cmbStream->Enabled = false;


            this->lblStreamInfo->Text =
                L"Select an academic year, term and class "
                L"to view stream enrollment.";
        }


        // =========================================================
        // CANCEL / BACK
        // =========================================================

        System::Void btnCancel_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->Close();
        }


    private:

        // =========================================================
        // EVENT WIRING
        // =========================================================

        void WireEvents()
        {
            this->btnSave->Click +=
                gcnew System::EventHandler(
                    this,
                    &StudentRegistration::btnSave_Click
                );

            this->btnClear->Click +=
                gcnew System::EventHandler(
                    this,
                    &StudentRegistration::btnClear_Click
                );

            this->btnCancel->Click +=
                gcnew System::EventHandler(
                    this,
                    &StudentRegistration::btnCancel_Click
                );

            this->btnChoosePhoto->Click +=
                gcnew System::EventHandler(
                    this,
                    &StudentRegistration::btnChoosePhoto_Click
                );

            this->cmbAcademicYear->SelectedIndexChanged +=
                gcnew System::EventHandler(
                    this,
                    &StudentRegistration::cmbAcademicYear_SelectedIndexChanged
                );

            this->cmbTerm->SelectedIndexChanged +=
                gcnew System::EventHandler(
                    this,
                    &StudentRegistration::cmbTerm_SelectedIndexChanged
                );

            this->cmbClass->SelectedIndexChanged +=
                gcnew System::EventHandler(
                    this,
                    &StudentRegistration::cmbClass_SelectedIndexChanged
                );

            this->cmbStream->SelectedIndexChanged +=
                gcnew System::EventHandler(
                    this,
                    &StudentRegistration::cmbStream_SelectedIndexChanged
                );
        }


    public:

        // =========================================================
        // NEW STUDENT
        // =========================================================

        StudentRegistration()
        {
            InitializeComponent();
            WireEvents();

            LoadAcademicYears();
            LoadClasses();
        }


        // =========================================================
        // EDIT EXISTING STUDENT
        // =========================================================

        StudentRegistration(long long studentId)
        {
            InitializeComponent();
            WireEvents();

            this->editMode = true;
            this->editingStudentId = studentId;

            this->Text =
                L"Edit Student";

            this->lblTitle->Text =
                L"Edit Student";

            this->lblSubtitle->Text =
                L"Loading student information...";

            LoadAcademicYears();
            LoadClasses();
            LoadStudentForEdit(studentId);
        }
    };
}