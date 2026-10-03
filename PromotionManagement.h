#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <stdexcept>

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;

    public ref class PromotionManagement : public Form
    {
    public:
        PromotionManagement(long long studentId)
        {
            this->studentId = studentId;

            InitializeComponent();
            ThemeManager::ApplyToForm(this);

            if (System::ComponentModel::LicenseManager::UsageMode !=
                System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadStudentAndEnrollment();
                LoadAcademicYears();
                LoadTerms();
                LoadTargetClasses();
            }
        }

    private:
        long long studentId = 0;
        int currentEnrollmentId = 0;
        int currentClassId = 0;
        int targetClassId = 0;

        TableLayoutPanel^ mainLayout;
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblStudent;
        Label^ lblCurrent;
        Label^ lblTarget;
        Label^ lblOption1;
        Label^ lblOption2;

        TextBox^ txtSearchStudent;
        Button^ btnSearchStudent;

        ComboBox^ cmbAcademicYear;
        ComboBox^ cmbTerm;
        ComboBox^ cmbClass;
        ComboBox^ cmbStream;
        ComboBox^ cmbOption1;
        ComboBox^ cmbOption2;

        Button^ btnPromote;
        Button^ btnCancel;

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
            this->Text = L"Student Promotion";
            this->StartPosition = FormStartPosition::CenterParent;
            this->MinimumSize = System::Drawing::Size(760, 560);
            this->ClientSize = System::Drawing::Size(900, 650);
            this->BackColor = Color::WhiteSmoke;

            this->mainLayout = gcnew TableLayoutPanel();
            this->mainLayout->Dock = DockStyle::Fill;
            this->mainLayout->Padding = Padding(28);
            this->mainLayout->ColumnCount = 2;
            this->mainLayout->RowCount = 10;
            this->mainLayout->BackColor = Color::White;

            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 180.0F));
            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 100.0F));

            for (int i = 0; i < 10; ++i)
            {
                this->mainLayout->RowStyles->Add(
                    gcnew RowStyle(SizeType::Absolute, 48.0F));
            }

            this->lblTitle = gcnew Label();
            this->lblTitle->Text = L"Student Promotion";
            this->lblTitle->Font =
                gcnew Drawing::Font(L"Segoe UI Semibold", 19.0F, FontStyle::Bold);
            this->lblTitle->ForeColor = Color::FromArgb(30, 41, 59);
            this->lblTitle->Dock = DockStyle::Fill;
            this->lblTitle->TextAlign = ContentAlignment::MiddleLeft;
            this->mainLayout->Controls->Add(this->lblTitle, 0, 0);
            this->mainLayout->SetColumnSpan(this->lblTitle, 2);

            this->lblSubtitle = gcnew Label();
            this->lblSubtitle->Text =
                L"Promote an existing student from S2 to S3 and record two optional subjects.";
            this->lblSubtitle->Font =
                gcnew Drawing::Font(L"Segoe UI", 9.5F);
            this->lblSubtitle->ForeColor = Color::DimGray;
            this->lblSubtitle->Dock = DockStyle::Fill;
            this->lblSubtitle->TextAlign = ContentAlignment::MiddleLeft;
            this->mainLayout->Controls->Add(this->lblSubtitle, 0, 1);
            this->mainLayout->SetColumnSpan(this->lblSubtitle, 2);

            this->txtSearchStudent = gcnew TextBox();
            this->txtSearchStudent->Dock = DockStyle::Fill;
            this->txtSearchStudent->Height = 32;
            this->txtSearchStudent->Margin = Padding(0, 7, 8, 7);

            this->btnSearchStudent = gcnew Button();
            this->btnSearchStudent->Text = L"Search";
            this->btnSearchStudent->Width = 90;
            this->btnSearchStudent->Dock = DockStyle::Right;
            this->btnSearchStudent->Click +=
                gcnew EventHandler(this, &PromotionManagement::btnSearchStudent_Click);

            FlowLayoutPanel^ searchPanel = gcnew FlowLayoutPanel();
            searchPanel->Dock = DockStyle::Fill;
            searchPanel->FlowDirection = FlowDirection::LeftToRight;
            searchPanel->WrapContents = false;
            searchPanel->Controls->Add(this->txtSearchStudent);
            searchPanel->Controls->Add(this->btnSearchStudent);

            this->mainLayout->Controls->Add(
                MakeLabel(L"Student Search"), 0, 2);
            this->mainLayout->Controls->Add(searchPanel, 1, 2);

            this->lblStudent = MakeValueLabel(L"No student loaded.");
            this->mainLayout->Controls->Add(MakeLabel(L"Student"), 0, 3);
            this->mainLayout->Controls->Add(this->lblStudent, 1, 3);

            this->lblCurrent = MakeValueLabel(L"No current enrollment loaded.");
            this->mainLayout->Controls->Add(MakeLabel(L"Current Class"), 0, 4);
            this->mainLayout->Controls->Add(this->lblCurrent, 1, 4);

            this->cmbAcademicYear = MakeCombo();
            this->mainLayout->Controls->Add(MakeLabel(L"Target Academic Year"), 0, 5);
            this->mainLayout->Controls->Add(this->cmbAcademicYear, 1, 5);

            this->cmbTerm = MakeCombo();
            this->mainLayout->Controls->Add(MakeLabel(L"Target Term"), 0, 6);
            this->mainLayout->Controls->Add(this->cmbTerm, 1, 6);

            FlowLayoutPanel^ classPanel = gcnew FlowLayoutPanel();
            classPanel->Dock = DockStyle::Fill;
            classPanel->WrapContents = false;

            this->cmbClass = MakeCombo();
            this->cmbClass->Width = 250;
            this->cmbClass->SelectedIndexChanged +=
                gcnew EventHandler(this, &PromotionManagement::cmbClass_SelectedIndexChanged);

            this->cmbStream = MakeCombo();
            this->cmbStream->Width = 250;

            classPanel->Controls->Add(this->cmbClass);
            classPanel->Controls->Add(this->cmbStream);

            this->mainLayout->Controls->Add(MakeLabel(L"Target Class / Stream"), 0, 7);
            this->mainLayout->Controls->Add(classPanel, 1, 7);

            this->cmbOption1 = MakeCombo();
            this->cmbOption2 = MakeCombo();

            FlowLayoutPanel^ optionPanel = gcnew FlowLayoutPanel();
            optionPanel->Dock = DockStyle::Fill;
            optionPanel->WrapContents = false;

            this->cmbOption1->Width = 250;
            this->cmbOption2->Width = 250;

            this->cmbOption1->SelectedIndexChanged +=
                gcnew EventHandler(this, &PromotionManagement::cmbOption1_SelectedIndexChanged);

            optionPanel->Controls->Add(this->cmbOption1);
            optionPanel->Controls->Add(this->cmbOption2);

            this->mainLayout->Controls->Add(
                MakeLabel(L"Two Optional Subjects"), 0, 8);
            this->mainLayout->Controls->Add(optionPanel, 1, 8);

            FlowLayoutPanel^ buttonPanel = gcnew FlowLayoutPanel();
            buttonPanel->Dock = DockStyle::Fill;
            buttonPanel->FlowDirection = FlowDirection::RightToLeft;
            buttonPanel->WrapContents = false;

            this->btnPromote = gcnew Button();
            this->btnPromote->Text = L"Promote to S3";
            this->btnPromote->Width = 150;
            this->btnPromote->Height = 36;
            this->btnPromote->Click +=
                gcnew EventHandler(this, &PromotionManagement::btnPromote_Click);

            this->btnCancel = gcnew Button();
            this->btnCancel->Text = L"Cancel";
            this->btnCancel->Width = 100;
            this->btnCancel->Height = 36;
            this->btnCancel->DialogResult = DialogResult::Cancel;

            buttonPanel->Controls->Add(this->btnPromote);
            buttonPanel->Controls->Add(this->btnCancel);

            this->mainLayout->Controls->Add(buttonPanel, 0, 9);
            this->mainLayout->SetColumnSpan(buttonPanel, 2);

            this->Controls->Add(this->mainLayout);
        }

        Label^ MakeLabel(String^ text)
        {
            Label^ label = gcnew Label();
            label->Text = text;
            label->Dock = DockStyle::Fill;
            label->Font = gcnew Drawing::Font(
                L"Segoe UI Semibold", 9.5F, FontStyle::Bold);
            label->ForeColor = Color::FromArgb(71, 85, 105);
            label->TextAlign = ContentAlignment::MiddleLeft;
            label->Margin = Padding(0, 2, 12, 2);
            return label;
        }

        Label^ MakeValueLabel(String^ text)
        {
            Label^ label = gcnew Label();
            label->Text = text;
            label->Dock = DockStyle::Fill;
            label->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            label->ForeColor = Color::FromArgb(30, 41, 59);
            label->TextAlign = ContentAlignment::MiddleLeft;
            return label;
        }

        ComboBox^ MakeCombo()
        {
            ComboBox^ combo = gcnew ComboBox();
            combo->Dock = DockStyle::Fill;
            combo->DropDownStyle = ComboBoxStyle::DropDownList;
            combo->Height = 32;
            combo->Margin = Padding(0, 7, 8, 7);
            return combo;
        }

        void LoadStudentAndEnrollment()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "s.registration_number, "
                        "s.first_name, "
                        "s.middle_name, "
                        "s.last_name, "
                        "e.enrollment_id, "
                        "e.class_id, "
                        "c.class_name "
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
                        "LEFT JOIN classes c ON c.class_id = e.class_id "
                        "WHERE s.student_id = ? "
                        "LIMIT 1"
                    )
                );

                stmt->setInt64(1, this->studentId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                if (!result->next())
                {
                    MessageBox::Show(
                        L"Student record could not be found.",
                        L"Promotion",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning);
                    return;
                }

                this->currentEnrollmentId =
                    result->isNull("enrollment_id")
                        ? 0
                        : result->getInt("enrollment_id");

                this->currentClassId =
                    result->isNull("class_id")
                        ? 0
                        : result->getInt("class_id");

                String^ first =
                    gcnew String(result->getString("first_name").c_str());

                String^ middle = L"";
                if (!result->isNull("middle_name"))
                {
                    middle =
                        gcnew String(result->getString("middle_name").c_str());
                }

                String^ last =
                    gcnew String(result->getString("last_name").c_str());

                String^ reg =
                    gcnew String(result->getString("registration_number").c_str());

                String^ fullName =
                    first +
                    (String::IsNullOrWhiteSpace(middle) ? L" " : L" " + middle + L" ") +
                    last;

                this->lblStudent->Text =
                    fullName + L"  [" + reg + L"]";

                this->lblCurrent->Text =
                    result->isNull("class_name")
                        ? L"No active enrollment"
                        : gcnew String(result->getString("class_name").c_str());
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
                    stmt->executeQuery());

                this->cmbAcademicYear->Items->Clear();

                while (result->next())
                {
                    this->cmbAcademicYear->Items->Add(
                        gcnew ComboItem(
                            result->getInt("academic_year_id"),
                            gcnew String(result->getString("year_name").c_str())
                        )
                    );
                }

                if (this->cmbAcademicYear->Items->Count > 0)
                {
                    this->cmbAcademicYear->SelectedIndex = 0;
                    LoadOptionalSubjects();
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

        void LoadTerms()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT term_id, term_name "
                        "FROM terms "
                        "WHERE status = 'Active' "
                        "ORDER BY term_id"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                this->cmbTerm->Items->Clear();

                while (result->next())
                {
                    this->cmbTerm->Items->Add(
                        gcnew ComboItem(
                            result->getInt("term_id"),
                            gcnew String(result->getString("term_name").c_str())
                        )
                    );
                }

                if (this->cmbTerm->Items->Count > 0)
                {
                    this->cmbTerm->SelectedIndex = 0;
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

        void LoadTargetClasses()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT c.class_id, c.class_name "
                        "FROM classes c "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "WHERE c.status = 'Active' "
                        "AND al.level_code = 'O_LEVEL' "
                        "ORDER BY c.class_id"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                this->cmbClass->Items->Clear();

                while (result->next())
                {
                    this->cmbClass->Items->Add(
                        gcnew ComboItem(
                            result->getInt("class_id"),
                            gcnew String(result->getString("class_name").c_str())
                        )
                    );
                }

                if (this->cmbClass->Items->Count > 0)
                {
                    this->cmbClass->SelectedIndex = 0;
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

        void LoadStreams()
        {
            this->cmbStream->Items->Clear();

            if (this->cmbClass->SelectedIndex < 0)
            {
                return;
            }

            ComboItem^ classItem =
                safe_cast<ComboItem^>(this->cmbClass->SelectedItem);

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT stream_id, stream_name "
                        "FROM streams "
                        "WHERE class_id = ? "
                        "AND status = 'Active' "
                        "ORDER BY stream_id"
                    )
                );

                stmt->setInt(1, classItem->Id);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                while (result->next())
                {
                    this->cmbStream->Items->Add(
                        gcnew ComboItem(
                            result->getInt("stream_id"),
                            gcnew String(result->getString("stream_name").c_str())
                        )
                    );
                }

                if (this->cmbStream->Items->Count > 0)
                {
                    this->cmbStream->SelectedIndex = 0;
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

        void LoadOptionalSubjects()
        {
            this->cmbOption1->Items->Clear();
            this->cmbOption2->Items->Clear();

            if (this->cmbAcademicYear->SelectedIndex < 0)
            {
                return;
            }

            ComboItem^ yearItem =
                safe_cast<ComboItem^>(this->cmbAcademicYear->SelectedItem);

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "s.subject_id, "
                        "s.subject_name "
                        "FROM academic_years ay "
                        "INNER JOIN curricula c "
                        "ON c.curriculum_id = ay.curriculum_id "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "INNER JOIN curriculum_subjects cs "
                        "ON cs.curriculum_id = c.curriculum_id "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = cs.subject_id "
                        "WHERE ay.academic_year_id = ? "
                        "AND al.level_code = 'O_LEVEL' "
                        "AND cs.requirement_type = 'Optional' "
                        "AND cs.status = 'Active' "
                        "AND s.status = 'Active' "
                        "ORDER BY s.subject_name"
                    )
                );

                stmt->setInt(1, yearItem->Id);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                while (result->next())
                {
                    this->cmbOption1->Items->Add(
                        gcnew ComboItem(
                            result->getInt("subject_id"),
                            gcnew String(result->getString("subject_name").c_str())
                        )
                    );

                    this->cmbOption2->Items->Add(
                        gcnew ComboItem(
                            result->getInt("subject_id"),
                            gcnew String(result->getString("subject_name").c_str())
                        )
                    );
                }

                if (this->cmbOption1->Items->Count > 0)
                {
                    this->cmbOption1->SelectedIndex = 0;
                    this->cmbOption2->SelectedIndex =
                        this->cmbOption2->Items->Count > 1 ? 1 : -1;
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

        System::Void cmbClass_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            LoadStreams();
        }

        System::Void cmbOption1_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->cmbOption1->SelectedIndex < 0)
            {
                return;
            }

            ComboItem^ first =
                safe_cast<ComboItem^>(this->cmbOption1->SelectedItem);

            for (int i = 0; i < this->cmbOption2->Items->Count; ++i)
            {
                ComboItem^ second =
                    safe_cast<ComboItem^>(this->cmbOption2->Items[i]);

                if (second->Id != first->Id)
                {
                    this->cmbOption2->SelectedIndex = i;
                    return;
                }
            }
        }

        System::Void btnSearchStudent_Click(
            Object^ sender,
            EventArgs^ e)
        {
            String^ searchText = this->txtSearchStudent->Text->Trim();

            if (String::IsNullOrWhiteSpace(searchText))
            {
                MessageBox::Show(
                    L"Enter a registration number or student name.",
                    L"Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT student_id "
                        "FROM students "
                        "WHERE registration_number LIKE ? "
                        "OR first_name LIKE ? "
                        "OR middle_name LIKE ? "
                        "OR last_name LIKE ? "
                        "OR CONCAT_WS(' ', first_name, middle_name, last_name) LIKE ? "
                        "ORDER BY student_id DESC "
                        "LIMIT 1"
                    )
                );

                std::string pattern =
                    "%" +
                    msclr::interop::marshal_as<std::string>(searchText) +
                    "%";

                for (int i = 1; i <= 5; ++i)
                {
                    stmt->setString(i, pattern);
                }

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                if (!result->next())
                {
                    MessageBox::Show(
                        L"No matching student was found.",
                        L"Promotion",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information);
                    return;
                }

                this->studentId = result->getInt64("student_id");
                this->currentEnrollmentId = 0;
                this->currentClassId = 0;

                LoadStudentAndEnrollment();
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

        System::Void btnPromote_Click(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->studentId <= 0 ||
                this->currentEnrollmentId <= 0)
            {
                MessageBox::Show(
                    L"Load a student with an active enrollment first.",
                    L"Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            ComboItem^ yearItem =
                this->cmbAcademicYear->SelectedIndex >= 0
                ? safe_cast<ComboItem^>(this->cmbAcademicYear->SelectedItem)
                : nullptr;

            ComboItem^ termItem =
                this->cmbTerm->SelectedIndex >= 0
                ? safe_cast<ComboItem^>(this->cmbTerm->SelectedItem)
                : nullptr;

            ComboItem^ classItem =
                this->cmbClass->SelectedIndex >= 0
                ? safe_cast<ComboItem^>(this->cmbClass->SelectedItem)
                : nullptr;

            ComboItem^ streamItem =
                this->cmbStream->SelectedIndex >= 0
                ? safe_cast<ComboItem^>(this->cmbStream->SelectedItem)
                : nullptr;

            ComboItem^ option1 =
                this->cmbOption1->SelectedIndex >= 0
                ? safe_cast<ComboItem^>(this->cmbOption1->SelectedItem)
                : nullptr;

            ComboItem^ option2 =
                this->cmbOption2->SelectedIndex >= 0
                ? safe_cast<ComboItem^>(this->cmbOption2->SelectedItem)
                : nullptr;

            if (yearItem == nullptr ||
                termItem == nullptr ||
                classItem == nullptr ||
                streamItem == nullptr ||
                option1 == nullptr ||
                option2 == nullptr)
            {
                MessageBox::Show(
                    L"Select the target academic year, term, class, stream and two optional subjects.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            if (option1->Id == option2->Id)
            {
                MessageBox::Show(
                    L"The two optional subjects must be different.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            auto con = DbConnection::GetConnection();

            try
            {
                con->setAutoCommit(false);

                // Confirm the current class is Senior 2.
                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT class_name "
                            "FROM classes "
                            "WHERE class_id = ? "
                            "LIMIT 1"
                        )
                    );
                    stmt->setInt(1, this->currentClassId);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery());

                    String^ className = result->next()
                        ? gcnew String(result->getString("class_name").c_str())
                        : L"";

                    if (String::IsNullOrWhiteSpace(className) ||
                        !className->Equals(
                            L"Senior 2",
                            StringComparison::OrdinalIgnoreCase))
                    {
                        con->rollback();

                        MessageBox::Show(
                            L"This promotion screen currently handles the S2 to S3 transition only.",
                            L"Promotion",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Information);
                        return;
                    }
                }

                // Confirm the target is Senior 3.
                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT class_name "
                            "FROM classes "
                            "WHERE class_id = ? "
                            "LIMIT 1"
                        )
                    );
                    stmt->setInt(1, classItem->Id);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery());

                    String^ className = result->next()
                        ? gcnew String(result->getString("class_name").c_str())
                        : L"";

                    if (String::IsNullOrWhiteSpace(className) ||
                        !className->Equals(
                            L"Senior 3",
                            StringComparison::OrdinalIgnoreCase))
                    {
                        con->rollback();

                        MessageBox::Show(
                            L"Select Senior 3 as the target class.",
                            L"Promotion",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning);
                        return;
                    }
                }

                // Prevent duplicate enrollment for the target academic period.
                {
                    std::unique_ptr<sql::PreparedStatement> check(
                        con->prepareStatement(
                            "SELECT enrollment_id "
                            "FROM enrollments "
                            "WHERE student_id = ? "
                            "AND academic_year_id = ? "
                            "AND term_id = ? "
                            "LIMIT 1"
                        )
                    );

                    check->setInt64(1, this->studentId);
                    check->setInt(2, yearItem->Id);
                    check->setInt(3, termItem->Id);

                    std::unique_ptr<sql::ResultSet> result(
                        check->executeQuery());

                    if (result->next())
                    {
                        con->rollback();

                        MessageBox::Show(
                            L"The student already has an enrollment for the selected academic year and term.",
                            L"Promotion",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning);
                        return;
                    }
                }

                std::unique_ptr<sql::PreparedStatement> insertEnrollment(
                    con->prepareStatement(
                        "INSERT INTO enrollments "
                        "(student_id, academic_year_id, term_id, class_id, stream_id, combination_id, enrollment_date, status) "
                        "VALUES (?, ?, ?, ?, ?, NULL, ?, 'Active')"
                    )
                );

                insertEnrollment->setInt64(1, this->studentId);
                insertEnrollment->setInt(2, yearItem->Id);
                insertEnrollment->setInt(3, termItem->Id);
                insertEnrollment->setInt(4, classItem->Id);
                insertEnrollment->setInt(5, streamItem->Id);
                insertEnrollment->setString(
                    6,
                    msclr::interop::marshal_as<std::string>(
                        DateTime::Today.ToString(L"yyyy-MM-dd")
                    )
                );

                insertEnrollment->executeUpdate();

                int newEnrollmentId = 0;

                std::unique_ptr<sql::Statement> keyStmt(
                    con->createStatement());

                std::unique_ptr<sql::ResultSet> keys(
                    keyStmt->executeQuery("SELECT LAST_INSERT_ID() AS enrollment_id"));

                if (!keys->next())
                {
                    con->rollback();
                    throw std::runtime_error("Could not obtain new enrollment ID.");
                }

                newEnrollmentId = keys->getInt("enrollment_id");

                std::unique_ptr<sql::PreparedStatement> insertOption(
                    con->prepareStatement(
                        "INSERT INTO enrollment_optional_subjects "
                        "(enrollment_id, subject_id, option_number, selected_date, status) "
                        "VALUES (?, ?, ?, ?, 'Active')"
                    )
                );

                for (int i = 0; i < 2; ++i)
                {
                    ComboItem^ selected =
                        (i == 0) ? option1 : option2;

                    insertOption->setInt(1, newEnrollmentId);
                    insertOption->setInt(2, selected->Id);
                    insertOption->setInt(3, i + 1);
                    insertOption->setString(
                        4,
                        msclr::interop::marshal_as<std::string>(
                            DateTime::Today.ToString(L"yyyy-MM-dd")
                        )
                    );
                    insertOption->executeUpdate();
                }

                // Keep the previous enrollment as history.
                std::unique_ptr<sql::PreparedStatement> closeOld(
                    con->prepareStatement(
                        "UPDATE enrollments "
                        "SET status = 'Completed' "
                        "WHERE enrollment_id = ? "
                        "AND student_id = ?"
                    )
                );

                closeOld->setInt(1, this->currentEnrollmentId);
                closeOld->setInt64(2, this->studentId);
                closeOld->executeUpdate();

                con->commit();
                con->setAutoCommit(true);

                MessageBox::Show(
                    L"The student has been promoted from Senior 2 to Senior 3 and the two optional subjects have been recorded.",
                    L"Promotion Complete",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);

                this->DialogResult = DialogResult::OK;
                this->Close();
            }
            catch (sql::SQLException& ex)
            {
                try
                {
                    con->rollback();
                    con->setAutoCommit(true);
                }
                catch (...)
                {
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
            catch (std::exception& ex)
            {
                try
                {
                    con->rollback();
                    con->setAutoCommit(true);
                }
                catch (...)
                {
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Promotion Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }
    };
}
