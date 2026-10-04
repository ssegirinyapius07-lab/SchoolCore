#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"
#include "AcademicContext.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;

    public ref class CurriculumSubjectsManagement : public Form
    {
    public:
        CurriculumSubjectsManagement()
        {
            InitializeComponent();
            ThemeManager::ApplyToForm(this);

            if (System::ComponentModel::LicenseManager::UsageMode !=
                System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadActiveYear();
                LoadCurricula();
            }
        }

    private:
        TableLayoutPanel^ mainLayout;
        ComboBox^ cmbCurriculum;
        Label^ lblActiveYear;
        TextBox^ txtSubject;
        TextBox^ txtUnebCode;
        TextBox^ txtSubjectGroup;
        ComboBox^ cmbRequirement;
        Button^ btnAdd;
        Button^ btnSave;
        DataGridView^ grid;

        ref class ComboItem
        {
        public:
            int Id;
            String^ Text;
            ComboItem(int id, String^ text)
            {
                Id = id;
                Text = text;
            }

            virtual String^ ToString() override
            {
                return Text;
            }
        };

        void InitializeComponent()
        {
            this->Text = L"Curriculum Subject Requirements";
            this->StartPosition =
                FormStartPosition::CenterScreen;
            this->WindowState =
                FormWindowState::Maximized;
            this->MinimumSize =
                System::Drawing::Size(1000, 650);
            this->BackColor =
                Color::FromArgb(248, 250, 252);

            this->mainLayout =
                gcnew TableLayoutPanel();
            this->mainLayout->Dock =
                DockStyle::Fill;
            this->mainLayout->Padding =
                System::Windows::Forms::Padding(24);
            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 4;

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    72.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    92.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    56.0F));

            Panel^ header =
                gcnew Panel();
            header->Dock =
                DockStyle::Fill;
            header->BackColor =
                Color::FromArgb(30, 41, 59);
            header->Padding =
                System::Windows::Forms::Padding(
                    18, 8, 18, 8);

            Label^ title =
                gcnew Label();
            title->Text =
                L"Curriculum Subject Requirements";
            title->Dock =
                DockStyle::Top;
            title->Height = 34;
            title->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    17.0F,
                    FontStyle::Bold);
            title->ForeColor =
                Color::White;
            title->TextAlign =
                ContentAlignment::MiddleLeft;

            Label^ subtitle =
                gcnew Label();
            subtitle->Text =
                L"Configure which subjects are Mandatory or Optional. Promotion uses this configuration.";
            subtitle->Dock =
                DockStyle::Fill;
            subtitle->ForeColor =
                Color::Gainsboro;
            subtitle->TextAlign =
                ContentAlignment::MiddleLeft;

            header->Controls->Add(subtitle);
            header->Controls->Add(title);

            this->mainLayout->Controls->Add(
                header,
                0,
                0);

            TableLayoutPanel^ setup =
                gcnew TableLayoutPanel();
            setup->Dock =
                DockStyle::Fill;
            setup->ColumnCount = 10;
            setup->RowCount = 2;
            setup->Padding =
                System::Windows::Forms::Padding(
                    0, 8, 0, 8);

            for (int i = 0; i < 10; ++i)
            {
                setup->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Percent,
                        10.0F));
            }

            Label^ yearLabel =
                gcnew Label();
            yearLabel->Text =
                L"Active Academic Year";
            yearLabel->Dock =
                DockStyle::Fill;
            yearLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblActiveYear =
                gcnew Label();
            this->lblActiveYear->Dock =
                DockStyle::Fill;
            this->lblActiveYear->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F,
                    FontStyle::Bold);
            this->lblActiveYear->TextAlign =
                ContentAlignment::MiddleLeft;

            Label^ curriculumLabel =
                gcnew Label();
            curriculumLabel->Text =
                L"Curriculum";
            curriculumLabel->Dock =
                DockStyle::Fill;
            curriculumLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbCurriculum =
                gcnew ComboBox();
            this->cmbCurriculum->Dock =
                DockStyle::Fill;
            this->cmbCurriculum->DropDownStyle =
                ComboBoxStyle::DropDownList;
            this->cmbCurriculum->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &CurriculumSubjectsManagement::cmbCurriculum_SelectedIndexChanged);

            Label^ subjectLabel =
                gcnew Label();
            subjectLabel->Text =
                L"Subject";
            subjectLabel->Dock =
                DockStyle::Fill;
            subjectLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->txtSubject =
                gcnew TextBox();
            this->txtSubject->Dock =
                DockStyle::Fill;
            this->txtSubject->ReadOnly = true;

            Label^ codeLabel =
                gcnew Label();
            codeLabel->Text =
                L"UNEB Code";
            codeLabel->Dock =
                DockStyle::Fill;
            codeLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->txtUnebCode =
                gcnew TextBox();
            this->txtUnebCode->Dock =
                DockStyle::Fill;

            Label^ groupLabel =
                gcnew Label();
            groupLabel->Text =
                L"Group";
            groupLabel->Dock =
                DockStyle::Fill;
            groupLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->txtSubjectGroup =
                gcnew TextBox();
            this->txtSubjectGroup->Dock =
                DockStyle::Fill;
            this->txtSubjectGroup->Text =
                L"Other";

            Label^ requirementLabel =
                gcnew Label();
            requirementLabel->Text =
                L"Requirement";
            requirementLabel->Dock =
                DockStyle::Fill;
            requirementLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbRequirement =
                gcnew ComboBox();
            this->cmbRequirement->Dock =
                DockStyle::Fill;
            this->cmbRequirement->DropDownStyle =
                ComboBoxStyle::DropDownList;
            this->cmbRequirement->Items->Add(L"Mandatory");
            this->cmbRequirement->Items->Add(L"Optional");
            this->cmbRequirement->SelectedIndex = 1;

            this->btnAdd =
                gcnew Button();
            this->btnAdd->Text =
                L"+ Add Subject";
            this->btnAdd->Dock =
                DockStyle::Fill;
            this->btnAdd->Click +=
                gcnew EventHandler(
                    this,
                    &CurriculumSubjectsManagement::btnAdd_Click);

            setup->Controls->Add(
                yearLabel, 0, 0);
            setup->Controls->Add(
                this->lblActiveYear, 1, 0);
            setup->SetColumnSpan(
                this->lblActiveYear, 2);

            setup->Controls->Add(
                curriculumLabel, 3, 0);
            setup->Controls->Add(
                this->cmbCurriculum, 4, 0);
            setup->SetColumnSpan(
                this->cmbCurriculum, 3);

            setup->Controls->Add(
                requirementLabel, 7, 0);
            setup->Controls->Add(
                this->cmbRequirement, 8, 0);
            setup->Controls->Add(
                this->btnAdd, 9, 0);

            setup->Controls->Add(
                subjectLabel, 0, 1);
            setup->Controls->Add(
                this->txtSubject, 1, 1);
            setup->SetColumnSpan(
                this->txtSubject, 2);

            setup->Controls->Add(
                codeLabel, 3, 1);
            setup->Controls->Add(
                this->txtUnebCode, 4, 1);
            setup->SetColumnSpan(
                this->txtUnebCode, 2);

            setup->Controls->Add(
                groupLabel, 6, 1);
            setup->Controls->Add(
                this->txtSubjectGroup, 7, 1);
            setup->SetColumnSpan(
                this->txtSubjectGroup, 3);

            this->mainLayout->Controls->Add(
                setup,
                0,
                1);

            this->grid =
                gcnew DataGridView();
            this->grid->Dock =
                DockStyle::Fill;
            this->grid->AllowUserToAddRows = false;
            this->grid->AllowUserToDeleteRows = false;
            this->grid->ReadOnly = false;
            this->grid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            this->grid->MultiSelect = false;
            this->grid->AutoGenerateColumns = false;
            this->grid->RowHeadersVisible = false;
            this->grid->BackgroundColor =
                Color::White;
            this->grid->ColumnHeadersHeight = 40;

            AddGridColumn(
                L"CurriculumSubjectId",
                L"ID",
                60);
            AddGridColumn(
                L"Subject",
                L"Subject",
                240);
            AddGridColumn(
                L"UNEBCode",
                L"UNEB Code",
                130);
            AddGridColumn(
                L"SubjectGroup",
                L"Subject Group",
                180);

            DataGridViewComboBoxColumn^ requirementColumn =
                gcnew DataGridViewComboBoxColumn();
            requirementColumn->Name =
                L"Requirement";
            requirementColumn->HeaderText =
                L"Requirement";
            requirementColumn->Width = 150;
            requirementColumn->Items->Add(
                L"Mandatory");
            requirementColumn->Items->Add(
                L"Optional");
            requirementColumn->FlatStyle =
                FlatStyle::Flat;
            this->grid->Columns->Add(
                requirementColumn);

            AddGridColumn(
                L"Status",
                L"Status",
                100);

            this->mainLayout->Controls->Add(
                this->grid,
                0,
                2);

            FlowLayoutPanel^ footer =
                gcnew FlowLayoutPanel();
            footer->Dock =
                DockStyle::Fill;
            footer->FlowDirection =
                FlowDirection::RightToLeft;
            footer->WrapContents = false;

            this->btnSave =
                gcnew Button();
            this->btnSave->Text =
                L"Save Requirements";
            this->btnSave->Width = 170;
            this->btnSave->Height = 38;
            this->btnSave->Click +=
                gcnew EventHandler(
                    this,
                    &CurriculumSubjectsManagement::btnSave_Click);

            Button^ closeButton =
                gcnew Button();
            closeButton->Text =
                L"Close";
            closeButton->Width = 100;
            closeButton->Height = 38;
            closeButton->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            footer->Controls->Add(
                closeButton);
            footer->Controls->Add(
                this->btnSave);

            this->mainLayout->Controls->Add(
                footer,
                0,
                3);

            this->Controls->Add(
                this->mainLayout);
        }

        void AddGridColumn(
            String^ name,
            String^ header,
            int width)
        {
            DataGridViewTextBoxColumn^ column =
                gcnew DataGridViewTextBoxColumn();

            column->Name = name;
            column->HeaderText = header;
            column->Width = width;
            column->ReadOnly =
                name == L"CurriculumSubjectId" ||
                name == L"Subject" ||
                name == L"Status";

            if (name == L"CurriculumSubjectId")
            {
                column->Visible = false;
            }

            this->grid->Columns->Add(column);
        }

        void LoadActiveYear()
        {
            try
            {
                AcademicYearInfo^ year =
                    AcademicContext::GetActiveAcademicYear();

                this->lblActiveYear->Text =
                    year->Name;
            }
            catch (std::exception& ex)
            {
                this->lblActiveYear->Text =
                    L"No active academic year";
            }
        }

        void LoadCurricula()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "c.curriculum_id, "
                        "c.curriculum_name, "
                        "al.level_code "
                        "FROM curricula c "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "WHERE c.status = 'Active' "
                        "AND al.status = 'Active' "
                        "ORDER BY al.level_code, c.curriculum_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                this->cmbCurriculum->Items->Clear();

                while (result->next())
                {
                    this->cmbCurriculum->Items->Add(
                        gcnew ComboItem(
                            result->getInt("curriculum_id"),
                            gcnew String(
                                result->getString(
                                    "curriculum_name").c_str())
                        )
                    );
                }

                if (this->cmbCurriculum->Items->Count > 0)
                {
                    this->cmbCurriculum->SelectedIndex = 0;
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        void LoadSubjectsInCurriculum()
        {
            this->grid->Rows->Clear();

            ComboItem^ curriculum =
                dynamic_cast<ComboItem^>(
                    this->cmbCurriculum->SelectedItem);

            if (curriculum == nullptr)
            {
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "cs.curriculum_subject_id, "
                        "s.subject_name, "
                        "cs.uneb_subject_code, "
                        "cs.subject_group, "
                        "cs.requirement_type, "
                        "cs.status "
                        "FROM curriculum_subjects cs "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = cs.subject_id "
                        "WHERE cs.curriculum_id = ? "
                        "ORDER BY s.subject_name"
                    )
                );

                stmt->setInt(
                    1,
                    curriculum->Id);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                while (result->next())
                {
                    this->grid->Rows->Add(
                        result->getInt(
                            "curriculum_subject_id"),
                        gcnew String(
                            result->getString(
                                "subject_name").c_str()),
                        gcnew String(
                            result->getString(
                                "uneb_subject_code").c_str()),
                        gcnew String(
                            result->getString(
                                "subject_group").c_str()),
                        gcnew String(
                            result->getString(
                                "requirement_type").c_str()),
                        gcnew String(
                            result->getString(
                                "status").c_str())
                    );
                }

                this->grid->Columns["UNEBCode"]->ReadOnly = true;
                this->grid->Columns["SubjectGroup"]->ReadOnly = true;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        System::Void cmbCurriculum_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            LoadSubjectsInCurriculum();
        }

        System::Void btnAdd_Click(
            Object^ sender,
            EventArgs^ e)
        {
            ComboItem^ curriculum =
                dynamic_cast<ComboItem^>(
                    this->cmbCurriculum->SelectedItem);

            if (curriculum == nullptr)
            {
                MessageBox::Show(
                    L"Select a curriculum first.",
                    L"Curriculum Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            String^ subjectName =
                this->txtSubject->Text->Trim();

            String^ code =
                this->txtUnebCode->Text->Trim();

            String^ group =
                this->txtSubjectGroup->Text->Trim();

            if (String::IsNullOrWhiteSpace(subjectName))
            {
                MessageBox::Show(
                    L"Subject selection is not configured yet. Use the Subjects module to select a subject, then add it here.",
                    L"Curriculum Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);
                return;
            }

            if (String::IsNullOrWhiteSpace(code))
            {
                MessageBox::Show(
                    L"Enter the official UNEB subject code.",
                    L"Curriculum Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            // This form deliberately does not invent subjects or UNEB codes.
            // A subject must first exist in the general subjects table.
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> findSubject(
                    con->prepareStatement(
                        "SELECT subject_id "
                        "FROM subjects "
                        "WHERE subject_name = ? "
                        "AND status = 'Active' "
                        "LIMIT 1"
                    )
                );

                findSubject->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        subjectName));

                std::unique_ptr<sql::ResultSet> subjectResult(
                    findSubject->executeQuery());

                if (!subjectResult->next())
                {
                    MessageBox::Show(
                        L"The subject must first be created in the Subjects module.",
                        L"Curriculum Subjects",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning);
                    return;
                }

                int subjectId =
                    subjectResult->getInt("subject_id");

                std::unique_ptr<sql::PreparedStatement> insert(
                    con->prepareStatement(
                        "INSERT INTO curriculum_subjects "
                        "(curriculum_id, subject_id, uneb_subject_code, subject_group, requirement_type, status) "
                        "VALUES (?, ?, ?, ?, ?, 'Active')"
                    )
                );

                insert->setInt(1, curriculum->Id);
                insert->setInt(2, subjectId);
                insert->setString(
                    3,
                    msclr::interop::marshal_as<std::string>(
                        code));
                insert->setString(
                    4,
                    msclr::interop::marshal_as<std::string>(
                        String::IsNullOrWhiteSpace(group)
                            ? L"Other"
                            : group));
                insert->setString(
                    5,
                    this->cmbRequirement->SelectedIndex == 0
                        ? "Mandatory"
                        : "Optional");

                insert->executeUpdate();

                LoadSubjectsInCurriculum();

                this->txtSubject->Clear();
                this->txtUnebCode->Clear();

                MessageBox::Show(
                    L"Curriculum subject added.",
                    L"Curriculum Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        System::Void btnSave_Click(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->grid->Rows->Count == 0)
            {
                MessageBox::Show(
                    L"There are no curriculum subjects to save.",
                    L"Curriculum Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                con->setAutoCommit(false);

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE curriculum_subjects "
                        "SET requirement_type = ? "
                        "WHERE curriculum_subject_id = ?"
                    )
                );

                for each (DataGridViewRow^ row in
                    this->grid->Rows)
                {
                    if (row->IsNewRow)
                    {
                        continue;
                    }

                    Object^ requirement =
                        row->Cells["Requirement"]->Value;

                    if (requirement == nullptr)
                    {
                        throw gcnew InvalidOperationException(
                            L"Every curriculum subject must have a requirement type.");
                    }

                    stmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(
                            Convert::ToString(requirement)));

                    stmt->setInt(
                        2,
                        Convert::ToInt32(
                            row->Cells["CurriculumSubjectId"]->Value));

                    stmt->executeUpdate();
                }

                con->commit();
                con->setAutoCommit(true);

                MessageBox::Show(
                    L"Requirement settings saved.",
                    L"Curriculum Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);

                LoadSubjectsInCurriculum();
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
            catch (InvalidOperationException^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
            }
        }
    };
}
