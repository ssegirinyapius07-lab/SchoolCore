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
                LoadSubjects();
                LoadCurricula();
            }
        }

    private:
        TableLayoutPanel^ mainLayout;
        ComboBox^ cmbCurriculum;
        Label^ lblActiveYear;
        ComboBox^ cmbSubject;
        TextBox^ txtUnebCode;
        TextBox^ txtSubjectGroup;
        ComboBox^ cmbRequirement;
        Button^ btnAdd;
        Button^ btnSave;
        Button^ btnLoadUceCore;
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
                L"Load verified UCE core subject codes once; use manual setup for other curriculum subjects.";
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

            this->cmbSubject =
                gcnew ComboBox();
            this->cmbSubject->Dock =
                DockStyle::Fill;
            this->cmbSubject->DropDownStyle =
                ComboBoxStyle::DropDownList;

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
                this->cmbSubject, 1, 1);
            setup->SetColumnSpan(
                this->cmbSubject, 2);

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

            this->btnLoadUceCore =
                gcnew Button();
            this->btnLoadUceCore->Text =
                L"Load UCE Core Codes";
            this->btnLoadUceCore->Width = 205;
            this->btnLoadUceCore->Height = 38;
            this->btnLoadUceCore->Margin =
                System::Windows::Forms::Padding(0, 0, 10, 0);
            this->btnLoadUceCore->BackColor =
                Color::FromArgb(226, 232, 240);
            this->btnLoadUceCore->ForeColor =
                Color::FromArgb(30, 41, 59);
            this->btnLoadUceCore->FlatStyle =
                FlatStyle::Flat;
            this->btnLoadUceCore->FlatAppearance->BorderSize = 0;
            this->btnLoadUceCore->Click +=
                gcnew EventHandler(
                    this,
                    &CurriculumSubjectsManagement::btnLoadUceCore_Click);

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
            footer->Controls->Add(
                this->btnLoadUceCore);

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
            catch (std::exception&)
            {
                this->lblActiveYear->Text =
                    L"No active academic year";
            }
        }

        void LoadSubjects()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT subject_id, subject_name "
                        "FROM subjects "
                        "WHERE status = 'Active' "
                        "ORDER BY subject_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                this->cmbSubject->Items->Clear();

                while (result->next())
                {
                    this->cmbSubject->Items->Add(
                        gcnew ComboItem(
                            result->getInt("subject_id"),
                            gcnew String(
                                result->getString(
                                    "subject_name").c_str())
                        )
                    );
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
                        "ORDER BY CASE WHEN c.curriculum_code = 'UCE_CBC' THEN 0 ELSE 1 END, "
                        "al.level_code, c.curriculum_name"
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

            ComboItem^ subject =
                dynamic_cast<ComboItem^>(
                    this->cmbSubject->SelectedItem);

            String^ subjectName =
                subject == nullptr
                    ? L""
                    : subject->Text;

            String^ code =
                this->txtUnebCode->Text->Trim();

            String^ group =
                this->txtSubjectGroup->Text->Trim();

            if (subject == nullptr)
            {
                MessageBox::Show(
                    L"Select a subject.",
                    L"Curriculum Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
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
            // The subject must already exist in the general Subjects table.
            try
            {
                auto con =
                    DbConnection::GetConnection();

                int subjectId = subject->Id;

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

                this->cmbSubject->SelectedIndex = -1;
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

        int EnsureCatalogSubject(sql::Connection* connection, String^ subjectName)
        {
            int subjectId = 0;

            std::unique_ptr<sql::PreparedStatement> findStmt(
                connection->prepareStatement(
                    "SELECT subject_id FROM subjects "
                    "WHERE subject_name = ? LIMIT 1"));

            findStmt->setString(
                1,
                msclr::interop::marshal_as<std::string>(subjectName));

            std::unique_ptr<sql::ResultSet> found(findStmt->executeQuery());

            if (found->next())
            {
                subjectId = found->getInt("subject_id");
            }
            else
            {
                std::unique_ptr<sql::PreparedStatement> insertStmt(
                    connection->prepareStatement(
                        "INSERT INTO subjects "
                        "(subject_name, description, status) "
                        "VALUES (?, 'Standard UCE core subject', 'Active')"));

                insertStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(subjectName));
                insertStmt->executeUpdate();

                std::unique_ptr<sql::Statement> idStmt(
                    connection->createStatement());
                std::unique_ptr<sql::ResultSet> idResult(
                    idStmt->executeQuery(
                        "SELECT LAST_INSERT_ID() AS subject_id"));

                if (!idResult->next())
                    throw gcnew InvalidOperationException(
                        L"SchoolCore could not retrieve the new subject ID.");

                subjectId = idResult->getInt("subject_id");
            }

            std::unique_ptr<sql::PreparedStatement> activateStmt(
                connection->prepareStatement(
                    "UPDATE subjects SET status = 'Active' "
                    "WHERE subject_id = ?"));
            activateStmt->setInt(1, subjectId);
            activateStmt->executeUpdate();

            return subjectId;
        }

        void EnsureOLevelSubjectMapping(
            sql::Connection* connection,
            int subjectId,
            int academicLevelId)
        {
            int mappingId = 0;

            std::unique_ptr<sql::PreparedStatement> findStmt(
                connection->prepareStatement(
                    "SELECT subject_academic_level_id "
                    "FROM subject_academic_levels "
                    "WHERE subject_id = ? AND academic_level_id = ? "
                    "LIMIT 1"));

            findStmt->setInt(1, subjectId);
            findStmt->setInt(2, academicLevelId);

            std::unique_ptr<sql::ResultSet> found(findStmt->executeQuery());

            if (found->next())
                mappingId = found->getInt("subject_academic_level_id");

            if (mappingId > 0)
            {
                std::unique_ptr<sql::PreparedStatement> updateStmt(
                    connection->prepareStatement(
                        "UPDATE subject_academic_levels "
                        "SET status = 'Active' "
                        "WHERE subject_academic_level_id = ?"));
                updateStmt->setInt(1, mappingId);
                updateStmt->executeUpdate();
            }
            else
            {
                std::unique_ptr<sql::PreparedStatement> insertStmt(
                    connection->prepareStatement(
                        "INSERT INTO subject_academic_levels "
                        "(subject_id, academic_level_id, status) "
                        "VALUES (?, ?, 'Active')"));
                insertStmt->setInt(1, subjectId);
                insertStmt->setInt(2, academicLevelId);
                insertStmt->executeUpdate();
            }
        }

        void EnsureUceCoreCurriculumMapping(
            sql::Connection* connection,
            int curriculumId,
            int subjectId,
            String^ unebCode,
            String^ subjectGroup)
        {
            std::unique_ptr<sql::PreparedStatement> conflictStmt(
                connection->prepareStatement(
                    "SELECT s.subject_name "
                    "FROM curriculum_subjects cs "
                    "INNER JOIN subjects s ON s.subject_id = cs.subject_id "
                    "WHERE cs.curriculum_id = ? "
                    "AND cs.uneb_subject_code = ? "
                    "AND cs.subject_id <> ? "
                    "LIMIT 1"));

            conflictStmt->setInt(1, curriculumId);
            conflictStmt->setString(
                2,
                msclr::interop::marshal_as<std::string>(unebCode));
            conflictStmt->setInt(3, subjectId);

            std::unique_ptr<sql::ResultSet> conflict(
                conflictStmt->executeQuery());

            if (conflict->next())
            {
                String^ conflictingSubject =
                    gcnew String(conflict->getString("subject_name").c_str());

                throw gcnew InvalidOperationException(
                    L"UNEB code " + unebCode +
                    L" is already assigned to " + conflictingSubject +
                    L" in this curriculum. Resolve that duplicate before loading the template.");
            }

            int curriculumSubjectId = 0;

            std::unique_ptr<sql::PreparedStatement> findStmt(
                connection->prepareStatement(
                    "SELECT curriculum_subject_id "
                    "FROM curriculum_subjects "
                    "WHERE curriculum_id = ? AND subject_id = ? "
                    "LIMIT 1"));

            findStmt->setInt(1, curriculumId);
            findStmt->setInt(2, subjectId);

            std::unique_ptr<sql::ResultSet> found(findStmt->executeQuery());

            if (found->next())
                curriculumSubjectId =
                    found->getInt("curriculum_subject_id");

            if (curriculumSubjectId > 0)
            {
                std::unique_ptr<sql::PreparedStatement> updateStmt(
                    connection->prepareStatement(
                        "UPDATE curriculum_subjects "
                        "SET uneb_subject_code = ?, subject_group = ?, "
                        "requirement_type = 'Mandatory', status = 'Active' "
                        "WHERE curriculum_subject_id = ?"));

                updateStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(unebCode));
                updateStmt->setString(
                    2,
                    msclr::interop::marshal_as<std::string>(subjectGroup));
                updateStmt->setInt(3, curriculumSubjectId);
                updateStmt->executeUpdate();
            }
            else
            {
                std::unique_ptr<sql::PreparedStatement> insertStmt(
                    connection->prepareStatement(
                        "INSERT INTO curriculum_subjects "
                        "(curriculum_id, subject_id, uneb_subject_code, "
                        "subject_group, requirement_type, status) "
                        "VALUES (?, ?, ?, ?, 'Mandatory', 'Active')"));

                insertStmt->setInt(1, curriculumId);
                insertStmt->setInt(2, subjectId);
                insertStmt->setString(
                    3,
                    msclr::interop::marshal_as<std::string>(unebCode));
                insertStmt->setString(
                    4,
                    msclr::interop::marshal_as<std::string>(subjectGroup));
                insertStmt->executeUpdate();
            }
        }

        System::Void btnLoadUceCore_Click(
            Object^ sender,
            EventArgs^ e)
        {
            ComboItem^ curriculum =
                dynamic_cast<ComboItem^>(
                    this->cmbCurriculum->SelectedItem);

            if (curriculum == nullptr)
            {
                MessageBox::Show(
                    L"Select the UCE CBC curriculum first.",
                    L"UCE Core Setup",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);
                return;
            }

            std::unique_ptr<sql::Connection> con;
            bool transactionStarted = false;

            try
            {
                con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> curriculumStmt(
                    con->prepareStatement(
                        "SELECT c.curriculum_code, al.level_code, "
                        "al.academic_level_id "
                        "FROM curricula c "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "WHERE c.curriculum_id = ? "
                        "AND c.status = 'Active' "
                        "AND al.status = 'Active' "
                        "LIMIT 1"));

                curriculumStmt->setInt(1, curriculum->Id);

                std::unique_ptr<sql::ResultSet> curriculumResult(
                    curriculumStmt->executeQuery());

                if (!curriculumResult->next())
                {
                    MessageBox::Show(
                        L"The selected curriculum is no longer active.",
                        L"UCE Core Setup",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning);
                    return;
                }

                String^ curriculumCode =
                    gcnew String(
                        curriculumResult->getString("curriculum_code").c_str());
                String^ levelCode =
                    gcnew String(
                        curriculumResult->getString("level_code").c_str());
                int academicLevelId =
                    curriculumResult->getInt("academic_level_id");

                if (!String::Equals(
                        curriculumCode,
                        L"UCE_CBC",
                        StringComparison::OrdinalIgnoreCase) ||
                    !String::Equals(
                        levelCode,
                        L"O_LEVEL",
                        StringComparison::OrdinalIgnoreCase))
                {
                    MessageBox::Show(
                        L"This template is only for the UCE CBC O-Level curriculum. "
                        L"A-Level codes are not changed by this action.",
                        L"UCE Core Setup",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information);
                    return;
                }

                System::Windows::Forms::DialogResult choice = MessageBox::Show(
                    this,
                    L"SchoolCore will add any missing UCE core subjects and their O-Level links, "
                    L"then load or correct these verified UNEB core codes:\n\n"
                    L"112  English Language\n"
                    L"456  Mathematics\n"
                    L"241  History & Political Education\n"
                    L"273  Geography\n"
                    L"535  Physics\n"
                    L"553  Biology\n"
                    L"545  Chemistry\n\n"
                    L"These seven curriculum records will be set to Mandatory. "
                    L"Other subjects, papers, marks and results will not be changed. Continue?",
                    L"Load UCE Core Codes",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Question);

                if (choice != System::Windows::Forms::DialogResult::Yes)
                    return;

                array<String^>^ subjectNames = gcnew array<String^>(7);
                subjectNames[0] = L"English Language";
                subjectNames[1] = L"Mathematics";
                subjectNames[2] = L"History & Political Education";
                subjectNames[3] = L"Geography";
                subjectNames[4] = L"Physics";
                subjectNames[5] = L"Biology";
                subjectNames[6] = L"Chemistry";

                array<String^>^ codes = gcnew array<String^>(7);
                codes[0] = L"112";
                codes[1] = L"456";
                codes[2] = L"241";
                codes[3] = L"273";
                codes[4] = L"535";
                codes[5] = L"553";
                codes[6] = L"545";

                array<String^>^ groups = gcnew array<String^>(7);
                groups[0] = L"Languages";
                groups[1] = L"Mathematics";
                groups[2] = L"Humanities";
                groups[3] = L"Humanities";
                groups[4] = L"Sciences";
                groups[5] = L"Sciences";
                groups[6] = L"Sciences";

                con->setAutoCommit(false);
                transactionStarted = true;

                for (int i = 0; i < subjectNames->Length; ++i)
                {
                    int subjectId =
                        EnsureCatalogSubject(con.get(), subjectNames[i]);

                    EnsureOLevelSubjectMapping(
                        con.get(),
                        subjectId,
                        academicLevelId);

                    EnsureUceCoreCurriculumMapping(
                        con.get(),
                        curriculum->Id,
                        subjectId,
                        codes[i],
                        groups[i]);
                }

                con->commit();
                con->setAutoCommit(true);
                transactionStarted = false;

                LoadSubjects();
                LoadSubjectsInCurriculum();

                MessageBox::Show(
                    L"The seven UCE core subject codes are ready. "
                    L"Existing matching records were corrected safely, and missing O-Level subject links were added. "
                    L"Configure examination papers separately when their official structure is confirmed.",
                    L"UCE Core Setup",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);
            }
            catch (sql::SQLException& ex)
            {
                if (transactionStarted && con != nullptr)
                {
                    try
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                    }
                    catch (...) {}
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"UCE Core Setup",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
            catch (Exception^ ex)
            {
                if (transactionStarted && con != nullptr)
                {
                    try
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                    }
                    catch (...) {}
                }

                MessageBox::Show(
                    ex->Message,
                    L"UCE Core Setup",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
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
