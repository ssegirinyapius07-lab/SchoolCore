#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>
#include <memory>
#include <string>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class CombinationManagement : public System::Windows::Forms::Form
    {
    public:
        ref class SubjectItem
        {
        public:
            int Id;
            String^ Name;

            SubjectItem(int id, String^ name)
            {
                Id = id;
                Name = name;
            }

            virtual String^ ToString() override
            {
                return Name;
            }
        };

        ref class CombinationRow
        {
        public:
            int Id;
            String^ Code;
            String^ Name;
            String^ PrincipalSubjects;
            String^ Subsidiary;
            String^ Status;

            CombinationRow(
                int id,
                String^ code,
                String^ name,
                String^ principals,
                String^ subsidiary,
                String^ status)
            {
                Id = id;
                Code = code;
                Name = name;
                PrincipalSubjects = principals;
                Subsidiary = subsidiary;
                Status = status;
            }
        };

        CombinationManagement(void)
        {
            InitializeComponent();

            this->btnNew->Click +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::btnNew_Click
                );

            this->btnEdit->Click +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::btnEdit_Click
                );

            this->btnToggle->Click +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::btnToggle_Click
                );

            this->btnBack->Click +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::btnBack_Click
                );

            this->grid->SelectionChanged +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::grid_SelectionChanged
                );

            if (
                System::ComponentModel::LicenseManager::UsageMode !=
                System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadCombinations();
                UpdateActionState();
            }
        }

    protected:
        ~CombinationManagement()
        {
            if (this->components)
                delete this->components;
        }

    private:
        System::ComponentModel::Container^ components;

        System::Windows::Forms::TableLayoutPanel^ mainLayout;
        System::Windows::Forms::Panel^ headerPanel;
        System::Windows::Forms::Label^ lblTitle;
        System::Windows::Forms::Label^ lblSubtitle;

        System::Windows::Forms::Panel^ actionPanel;
        System::Windows::Forms::Button^ btnNew;
        System::Windows::Forms::Button^ btnEdit;
        System::Windows::Forms::Button^ btnToggle;

        System::Windows::Forms::DataGridView^ grid;
        System::Windows::Forms::FlowLayoutPanel^ bottomPanel;

        System::Windows::Forms::Form^ editorForm;
        System::Windows::Forms::TextBox^ editorCodeBox;
        System::Windows::Forms::TextBox^ editorNameBox;
        System::Windows::Forms::ComboBox^ editorPrincipal1;
        System::Windows::Forms::ComboBox^ editorPrincipal2;
        System::Windows::Forms::ComboBox^ editorPrincipal3;
        System::Windows::Forms::ComboBox^ editorSubsidiary;

        bool editorEditMode = false;
        int editingCombinationId = 0;

        System::Windows::Forms::Button^ btnBack;

        void ConfigureButton(
            System::Windows::Forms::Button^ button,
            String^ text)
        {
            button->Text = text;
            button->Width = 150;
            button->Height = 38;
            button->FlatStyle =
                System::Windows::Forms::FlatStyle::Flat;
            button->FlatAppearance->BorderSize = 0;
            button->Margin =
                System::Windows::Forms::Padding(0, 0, 10, 0);
            button->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F,
                    System::Drawing::FontStyle::Bold
                );
        }

        System::Windows::Forms::Label^ CreateFormLabel(
            String^ text)
        {
            System::Windows::Forms::Label^ label =
                gcnew System::Windows::Forms::Label();

            label->Text = text;
            label->Dock =
                System::Windows::Forms::DockStyle::Fill;
            label->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;
            label->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F,
                    System::Drawing::FontStyle::Bold
                );

            return label;
        }

        String^ NormalizeText(String^ value)
        {
            if (value == nullptr)
                return L"";

            return value->Trim();
        }

        String^ JoinThree(
            String^ a,
            String^ b,
            String^ c)
        {
            return a + L" + " + b + L" + " + c;
        }

        int GetSelectedCombinationId()
        {
            if (this->grid->SelectedRows->Count == 0)
                return 0;

            Object^ value =
                this->grid
                    ->SelectedRows[0]
                    ->Cells["CombinationId"]
                    ->Value;

            if (value == nullptr)
                return 0;

            return Convert::ToInt32(value);
        }

        void UpdateActionState()
        {
            bool selected =
                this->grid->SelectedRows->Count > 0;

            this->btnEdit->Enabled = selected;
            this->btnToggle->Enabled = selected;
        }

        void LoadCombinations()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "sc.combination_id, "
                        "sc.combination_code, "
                        "sc.combination_name, "
                        "sc.status, "
                        "GROUP_CONCAT("
                        "CASE WHEN cs.subject_role = 'Principal' "
                        "THEN s.subject_name END "
                        "ORDER BY cs.combination_subject_id "
                        "SEPARATOR ' + '"
                        ") AS principal_subjects, "
                        "MAX("
                        "CASE WHEN cs.subject_role = 'Subsidiary' "
                        "THEN s.subject_name END"
                        ") AS subsidiary "
                        "FROM subject_combinations sc "
                        "LEFT JOIN combination_subjects cs "
                        "ON cs.combination_id = sc.combination_id "
                        "AND cs.status = 'Active' "
                        "LEFT JOIN subjects s "
                        "ON s.subject_id = cs.subject_id "
                        "WHERE sc.academic_level_id = ("
                        "SELECT academic_level_id "
                        "FROM academic_levels "
                        "WHERE level_code = 'A_LEVEL' "
                        "LIMIT 1"
                        ") "
                        "GROUP BY "
                        "sc.combination_id, "
                        "sc.combination_code, "
                        "sc.combination_name, "
                        "sc.status "
                        "ORDER BY sc.combination_code ASC"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                this->grid->Rows->Clear();

                while (result->next())
                {
                    String^ principals =
                        result->isNull("principal_subjects")
                        ? L""
                        : gcnew String(
                            result->getString(
                                "principal_subjects"
                            ).c_str()
                        );

                    String^ subsidiary =
                        result->isNull("subsidiary")
                        ? L""
                        : gcnew String(
                            result->getString(
                                "subsidiary"
                            ).c_str()
                        );

                    this->grid->Rows->Add(
                        result->getInt("combination_id"),
                        gcnew String(
                            result->getString(
                                "combination_code"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "combination_name"
                            ).c_str()
                        ),
                        principals,
                        subsidiary,
                        gcnew String(
                            result->getString(
                                "status"
                            ).c_str()
                        )
                    );
                }

                UpdateActionState();
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

        void LoadSubjects(
            System::Windows::Forms::ComboBox^ combo,
            int selectedId)
        {
            combo->Items->Clear();
            combo->Items->Add(L"Select subject");

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT subject_id, subject_name "
                        "FROM subjects "
                        "WHERE status = 'Active' "
                        "ORDER BY subject_name ASC"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                int selectedIndex = 0;
                int index = 0;

                while (result->next())
                {
                    index++;

                    int id =
                        result->getInt("subject_id");

                    combo->Items->Add(
                        gcnew SubjectItem(
                            id,
                            gcnew String(
                                result->getString(
                                    "subject_name"
                                ).c_str()
                            )
                        )
                    );

                    if (id == selectedId)
                        selectedIndex = index;
                }

                combo->SelectedIndex = selectedIndex;
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

        wchar_t GetFirstLetter(String^ value)
        {
            if (String::IsNullOrWhiteSpace(value))
                return L'\0';

            for (int i = 0; i < value->Length; i++)
            {
                wchar_t ch = value[i];

                if (!Char::IsWhiteSpace(ch) &&
                    Char::IsLetter(ch))
                {
                    return Char::ToUpper(ch);
                }
            }

            return L'\0';
        }

        String^ BuildCombinationCode()
        {
            SubjectItem^ first =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal1->SelectedItem
                );

            SubjectItem^ second =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal2->SelectedItem
                );

            SubjectItem^ third =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal3->SelectedItem
                );

            if (first == nullptr ||
                second == nullptr ||
                third == nullptr)
            {
                return L"";
            }

            wchar_t a = GetFirstLetter(first->Name);
            wchar_t b = GetFirstLetter(second->Name);
            wchar_t c = GetFirstLetter(third->Name);

            if (a == L'\0' ||
                b == L'\0' ||
                c == L'\0')
            {
                return L"";
            }

            return
                gcnew String(a, 1) +
                gcnew String(b, 1) +
                gcnew String(c, 1);
        }

        String^ BuildCombinationName()
        {
            SubjectItem^ first =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal1->SelectedItem
                );

            SubjectItem^ second =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal2->SelectedItem
                );

            SubjectItem^ third =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal3->SelectedItem
                );

            if (first == nullptr ||
                second == nullptr ||
                third == nullptr)
            {
                return L"";
            }

            return
                first->Name +
                L", " +
                second->Name +
                L" and " +
                third->Name;
        }

        void UpdateGeneratedCombination()
        {
            this->editorCodeBox->Text =
                BuildCombinationCode();

            this->editorNameBox->Text =
                BuildCombinationName();
        }

        System::Void PrincipalSubjectChanged(
            Object^ sender,
            EventArgs^ e)
        {
            UpdateGeneratedCombination();
        }

        bool SaveCombination(
            int combinationId,
            bool editMode,
            String^ code,
            String^ name,
            SubjectItem^ principal1,
            SubjectItem^ principal2,
            SubjectItem^ principal3,
            SubjectItem^ subsidiary)
        {
            if (
                principal1 == nullptr ||
                principal2 == nullptr ||
                principal3 == nullptr ||
                subsidiary == nullptr)
            {
                MessageBox::Show(
                    L"Choose all three principal subjects and the subsidiary subject.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return false;
            }

            if (
                principal1->Id == principal2->Id ||
                principal1->Id == principal3->Id ||
                principal2->Id == principal3->Id ||
                principal1->Id == subsidiary->Id ||
                principal2->Id == subsidiary->Id ||
                principal3->Id == subsidiary->Id)
            {
                MessageBox::Show(
                    L"The same subject cannot be used more than once in a combination.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return false;
            }

            code = NormalizeText(code);
            name = NormalizeText(name);

            if (
                String::IsNullOrWhiteSpace(code) ||
                String::IsNullOrWhiteSpace(name))
            {
                MessageBox::Show(
                    L"Enter the combination code and full combination name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return false;
            }

            std::unique_ptr<sql::Connection> con;

            try
            {
                con = DbConnection::GetConnection();

                con->setAutoCommit(false);

                int aLevelId = 0;

                {
                    std::unique_ptr<sql::PreparedStatement> levelStmt(
                        con->prepareStatement(
                            "SELECT academic_level_id "
                            "FROM academic_levels "
                            "WHERE level_code = 'A_LEVEL' "
                            "LIMIT 1"
                        )
                    );

                    std::unique_ptr<sql::ResultSet> levelResult(
                        levelStmt->executeQuery()
                    );

                    if (!levelResult->next())
                    {
                        con->rollback();
                        con->setAutoCommit(true);

                        MessageBox::Show(
                            L"A-Level has not been configured in the database.",
                            L"Configuration",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning
                        );

                        return false;
                    }

                    aLevelId =
                        levelResult->getInt(
                            "academic_level_id"
                        );
                }

                int finalCombinationId = combinationId;

                if (!editMode)
                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "INSERT INTO subject_combinations "
                            "(academic_level_id, combination_code, combination_name, status) "
                            "VALUES (?, ?, ?, 'Active')"
                        )
                    );

                    stmt->setInt(1, aLevelId);
                    stmt->setString(
                        2,
                        msclr::interop::marshal_as<std::string>(code)
                    );
                    stmt->setString(
                        3,
                        msclr::interop::marshal_as<std::string>(name)
                    );

                    stmt->executeUpdate();

                    std::unique_ptr<sql::Statement> idStmt(
                        con->createStatement()
                    );

                    std::unique_ptr<sql::ResultSet> idResult(
                        idStmt->executeQuery(
                            "SELECT LAST_INSERT_ID() AS combination_id"
                        )
                    );

                    if (!idResult->next())
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                        return false;
                    }

                    finalCombinationId =
                        idResult->getInt("combination_id");
                }
                else
                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "UPDATE subject_combinations "
                            "SET combination_code = ?, "
                            "combination_name = ? "
                            "WHERE combination_id = ? "
                            "AND academic_level_id = ?"
                        )
                    );

                    stmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(code)
                    );
                    stmt->setString(
                        2,
                        msclr::interop::marshal_as<std::string>(name)
                    );
                    stmt->setInt(
                        3,
                        finalCombinationId
                    );
                    stmt->setInt(
                        4,
                        aLevelId
                    );

                    stmt->executeUpdate();

                    std::unique_ptr<sql::PreparedStatement> deleteStmt(
                        con->prepareStatement(
                            "DELETE FROM combination_subjects "
                            "WHERE combination_id = ?"
                        )
                    );

                    deleteStmt->setInt(
                        1,
                        finalCombinationId
                    );

                    deleteStmt->executeUpdate();
                }

                const int ids[4] = {
                    principal1->Id,
                    principal2->Id,
                    principal3->Id,
                    subsidiary->Id
                };

                const char* roles[4] = {
                    "Principal",
                    "Principal",
                    "Principal",
                    "Subsidiary"
                };

                for (int i = 0; i < 4; i++)
                {
                    std::unique_ptr<sql::PreparedStatement> subjectStmt(
                        con->prepareStatement(
                            "INSERT INTO combination_subjects "
                            "(combination_id, subject_id, subject_role, status) "
                            "VALUES (?, ?, ?, 'Active')"
                        )
                    );

                    subjectStmt->setInt(
                        1,
                        finalCombinationId
                    );
                    subjectStmt->setInt(
                        2,
                        ids[i]
                    );
                    subjectStmt->setString(
                        3,
                        roles[i]
                    );

                    subjectStmt->executeUpdate();
                }

                con->commit();
                con->setAutoCommit(true);

                return true;
            }
            catch (sql::SQLException& ex)
            {
                if (con != nullptr)
                {
                    try
                    {
                        con->rollback();
                        con->setAutoCommit(true);
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

                return false;
            }
        }

        void OpenEditor(int combinationId)
        {
            this->editorEditMode = combinationId > 0;
            this->editingCombinationId = combinationId;

            this->editorForm =
                gcnew System::Windows::Forms::Form();

            this->editorForm->Text =
                this->editorEditMode
                ? L"Edit A-Level Combination"
                : L"New A-Level Combination";

            this->editorForm->StartPosition =
                FormStartPosition::CenterParent;

            this->editorForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedSingle;

            this->editorForm->MaximizeBox = false;
            this->editorForm->MinimizeBox = false;
            this->editorForm->ShowInTaskbar = false;
            this->editorForm->ClientSize =
                System::Drawing::Size(
                    720,
                    560
                );

            System::Windows::Forms::TableLayoutPanel^ layout =
                gcnew System::Windows::Forms::TableLayoutPanel();

            layout->Dock = DockStyle::Fill;
            layout->Padding =
                System::Windows::Forms::Padding(24);
            layout->ColumnCount = 2;
            layout->RowCount = 8;

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    170.0F
                )
            );

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            for (int i = 0; i < 8; i++)
            {
                layout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        i == 7 ? 56.0F : 48.0F
                    )
                );
            }

            this->editorCodeBox =
                gcnew System::Windows::Forms::TextBox();

            this->editorNameBox =
                gcnew System::Windows::Forms::TextBox();

            this->editorPrincipal1 =
                gcnew System::Windows::Forms::ComboBox();

            this->editorPrincipal2 =
                gcnew System::Windows::Forms::ComboBox();

            this->editorPrincipal3 =
                gcnew System::Windows::Forms::ComboBox();

            this->editorSubsidiary =
                gcnew System::Windows::Forms::ComboBox();

            this->editorPrincipal1->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->editorPrincipal2->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->editorPrincipal3->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->editorSubsidiary->DropDownStyle =
                ComboBoxStyle::DropDownList;

            LoadSubjects(
                this->editorPrincipal1,
                0
            );

            LoadSubjects(
                this->editorPrincipal2,
                0
            );

            LoadSubjects(
                this->editorPrincipal3,
                0
            );

            LoadSubjects(
                this->editorSubsidiary,
                0
            );

            this->editorCodeBox->ReadOnly = true;
            this->editorNameBox->ReadOnly = true;

            this->editorCodeBox->BackColor =
                System::Drawing::Color::FromArgb(
                    248,
                    250,
                    252
                );

            this->editorNameBox->BackColor =
                System::Drawing::Color::FromArgb(
                    248,
                    250,
                    252
                );

            this->editorPrincipal1->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::PrincipalSubjectChanged
                );

            this->editorPrincipal2->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::PrincipalSubjectChanged
                );

            this->editorPrincipal3->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::PrincipalSubjectChanged
                );

            if (this->editorEditMode)
            {
                try
                {
                    auto con =
                        DbConnection::GetConnection();

                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "sc.combination_code, "
                            "sc.combination_name, "
                            "cs.subject_id, "
                            "cs.subject_role "
                            "FROM subject_combinations sc "
                            "INNER JOIN combination_subjects cs "
                            "ON cs.combination_id = sc.combination_id "
                            "WHERE sc.combination_id = ? "
                            "AND cs.status = 'Active' "
                            "ORDER BY cs.combination_subject_id ASC"
                        )
                    );

                    stmt->setInt(
                        1,
                        combinationId
                    );

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    int principalIndex = 0;
                    int principal1Id = 0;
                    int principal2Id = 0;
                    int principal3Id = 0;
                    int subsidiaryId = 0;

                    while (result->next())
                    {
                        this->editorCodeBox->Text =
                            gcnew String(
                                result->getString(
                                    "combination_code"
                                ).c_str()
                            );

                        this->editorNameBox->Text =
                            gcnew String(
                                result->getString(
                                    "combination_name"
                                ).c_str()
                            );

                        int subjectId =
                            result->getInt(
                                "subject_id"
                            );

                        String^ role =
                            gcnew String(
                                result->getString(
                                    "subject_role"
                                ).c_str()
                            );

                        if (role == L"Subsidiary")
                        {
                            subsidiaryId = subjectId;
                        }
                        else
                        {
                            principalIndex++;

                            if (principalIndex == 1)
                                principal1Id = subjectId;
                            else if (principalIndex == 2)
                                principal2Id = subjectId;
                            else if (principalIndex == 3)
                                principal3Id = subjectId;
                        }
                    }

                    for (int i = 1;
                         i < this->editorPrincipal1->Items->Count;
                         i++)
                    {
                        SubjectItem^ item1 =
                            dynamic_cast<SubjectItem^>(
                                this->editorPrincipal1->Items[i]
                            );

                        if (item1 != nullptr &&
                            item1->Id == principal1Id)
                            this->editorPrincipal1->SelectedIndex = i;

                        SubjectItem^ item2 =
                            dynamic_cast<SubjectItem^>(
                                this->editorPrincipal2->Items[i]
                            );

                        if (item2 != nullptr &&
                            item2->Id == principal2Id)
                            this->editorPrincipal2->SelectedIndex = i;

                        SubjectItem^ item3 =
                            dynamic_cast<SubjectItem^>(
                                this->editorPrincipal3->Items[i]
                            );

                        if (item3 != nullptr &&
                            item3->Id == principal3Id)
                            this->editorPrincipal3->SelectedIndex = i;

                        SubjectItem^ sub =
                            dynamic_cast<SubjectItem^>(
                                this->editorSubsidiary->Items[i]
                            );

                        if (sub != nullptr &&
                            sub->Id == subsidiaryId)
                            this->editorSubsidiary->SelectedIndex = i;
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

            UpdateGeneratedCombination();

            layout->Controls->Add(
                CreateFormLabel(L"Combination Code (Automatic)"),
                0,
                0
            );
            layout->Controls->Add(
                this->editorCodeBox,
                1,
                0
            );

            layout->Controls->Add(
                CreateFormLabel(L"Combination Name (Automatic)"),
                0,
                1
            );
            layout->Controls->Add(
                this->editorNameBox,
                1,
                1
            );

            layout->Controls->Add(
                CreateFormLabel(L"Principal Subject 1"),
                0,
                2
            );
            layout->Controls->Add(
                this->editorPrincipal1,
                1,
                2
            );

            layout->Controls->Add(
                CreateFormLabel(L"Principal Subject 2"),
                0,
                3
            );
            layout->Controls->Add(
                this->editorPrincipal2,
                1,
                3
            );

            layout->Controls->Add(
                CreateFormLabel(L"Principal Subject 3"),
                0,
                4
            );
            layout->Controls->Add(
                this->editorPrincipal3,
                1,
                4
            );

            layout->Controls->Add(
                CreateFormLabel(L"Subsidiary Subject"),
                0,
                5
            );
            layout->Controls->Add(
                this->editorSubsidiary,
                1,
                5
            );

            System::Windows::Forms::Label^ note =
                gcnew System::Windows::Forms::Label();

            note->Text =
                L"An A-Level combination contains three principal subjects and one subsidiary subject.";

            note->Dock = DockStyle::Fill;
            note->ForeColor =
                System::Drawing::Color::DimGray;
            note->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;

            layout->Controls->Add(
                note,
                0,
                6
            );

            layout->SetColumnSpan(
                note,
                2
            );

            System::Windows::Forms::FlowLayoutPanel^ buttons =
                gcnew System::Windows::Forms::FlowLayoutPanel();

            buttons->Dock =
                System::Windows::Forms::DockStyle::Fill;

            buttons->FlowDirection =
                System::Windows::Forms::FlowDirection::RightToLeft;

            buttons->WrapContents = false;

            System::Windows::Forms::Button^ save =
                gcnew System::Windows::Forms::Button();

            save->Text =
                this->editorEditMode
                ? L"Save Changes"
                : L"Save Combination";

            save->Width = 160;
            save->Height = 38;

            System::Windows::Forms::Button^ cancel =
                gcnew System::Windows::Forms::Button();

            cancel->Text = L"Cancel";
            cancel->Width = 110;
            cancel->Height = 38;

            buttons->Controls->Add(save);
            buttons->Controls->Add(cancel);

            layout->Controls->Add(
                buttons,
                0,
                7
            );

            layout->SetColumnSpan(
                buttons,
                2
            );

            cancel->Click +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::editorCancel_Click
                );

            save->Click +=
                gcnew EventHandler(
                    this,
                    &CombinationManagement::editorSave_Click
                );

            this->editorForm->Controls->Add(layout);

            this->editorForm->ShowDialog(this);

            this->editorForm = nullptr;
        }

        System::Void editorCancel_Click(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->editorForm != nullptr)
                this->editorForm->Close();
        }

        System::Void editorSave_Click(
            Object^ sender,
            EventArgs^ e)
        {
            SubjectItem^ item1 =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal1->SelectedItem
                );

            SubjectItem^ item2 =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal2->SelectedItem
                );

            SubjectItem^ item3 =
                dynamic_cast<SubjectItem^>(
                    this->editorPrincipal3->SelectedItem
                );

            SubjectItem^ subsidiary =
                dynamic_cast<SubjectItem^>(
                    this->editorSubsidiary->SelectedItem
                );

            if (
                this->SaveCombination(
                    this->editingCombinationId,
                    this->editorEditMode,
                    this->editorCodeBox->Text,
                    this->editorNameBox->Text,
                    item1,
                    item2,
                    item3,
                    subsidiary
                ))
            {
                if (this->editorForm != nullptr)
                    this->editorForm->Close();

                this->LoadCombinations();
            }
        }

        System::Void btnNew_Click(
            Object^ sender,
            EventArgs^ e)
        {
            OpenEditor(0);
        }

        System::Void btnEdit_Click(
            Object^ sender,
            EventArgs^ e)
        {
            int id = GetSelectedCombinationId();

            if (id > 0)
                OpenEditor(id);
        }

        System::Void btnToggle_Click(
            Object^ sender,
            EventArgs^ e)
        {
            int id = GetSelectedCombinationId();

            if (id == 0)
                return;

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE subject_combinations "
                        "SET status = CASE "
                        "WHEN status = 'Active' THEN 'Inactive' "
                        "ELSE 'Active' END "
                        "WHERE combination_id = ?"
                    )
                );

                stmt->setInt(1, id);
                stmt->executeUpdate();

                LoadCombinations();
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

        System::Void grid_SelectionChanged(
            Object^ sender,
            EventArgs^ e)
        {
            UpdateActionState();
        }

        System::Void btnBack_Click(
            Object^ sender,
            EventArgs^ e)
        {
            this->Close();
        }

#pragma region Windows Form Designer generated code

        void InitializeComponent(void)
        {
            this->components =
                gcnew System::ComponentModel::Container();

            this->mainLayout =
                gcnew System::Windows::Forms::TableLayoutPanel();

            this->headerPanel =
                gcnew System::Windows::Forms::Panel();

            this->lblTitle =
                gcnew System::Windows::Forms::Label();

            this->lblSubtitle =
                gcnew System::Windows::Forms::Label();

            this->actionPanel =
                gcnew System::Windows::Forms::FlowLayoutPanel();

            this->btnNew =
                gcnew System::Windows::Forms::Button();

            this->btnEdit =
                gcnew System::Windows::Forms::Button();

            this->btnToggle =
                gcnew System::Windows::Forms::Button();

            this->grid =
                gcnew System::Windows::Forms::DataGridView();

            this->bottomPanel =
                gcnew System::Windows::Forms::FlowLayoutPanel();

            this->btnBack =
                gcnew System::Windows::Forms::Button();

            this->SuspendLayout();

            this->Text =
                L"A-Level Combinations";

            this->StartPosition =
                System::Windows::Forms::FormStartPosition::CenterScreen;

            this->WindowState =
                System::Windows::Forms::FormWindowState::Maximized;

            this->MinimumSize =
                System::Drawing::Size(
                    950,
                    620
                );

            this->BackColor =
                System::Drawing::Color::FromArgb(
                    248,
                    250,
                    252
                );

            this->mainLayout->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 4;

            this->mainLayout->RowStyles->Add(
                gcnew System::Windows::Forms::RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    80.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew System::Windows::Forms::RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    58.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew System::Windows::Forms::RowStyle(
                    System::Windows::Forms::SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew System::Windows::Forms::RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    56.0F
                )
            );

            this->headerPanel->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->headerPanel->Padding =
                System::Windows::Forms::Padding(
                    20,
                    10,
                    20,
                    10
                );

            this->headerPanel->BackColor =
                System::Drawing::Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->lblTitle->Dock =
                System::Windows::Forms::DockStyle::Top;

            this->lblTitle->Height = 34;

            this->lblTitle->Text =
                L"A-Level Combinations";

            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    17.0F,
                    System::Drawing::FontStyle::Bold
                );

            this->lblTitle->ForeColor =
                System::Drawing::Color::White;

            this->lblTitle->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;

            this->lblSubtitle->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblSubtitle->Text =
                L"Configure three principal subjects and one subsidiary subject for each A-Level combination.";

            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->lblSubtitle->ForeColor =
                System::Drawing::Color::Gainsboro;

            this->lblSubtitle->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;

            this->headerPanel->Controls->Add(
                this->lblSubtitle
            );

            this->headerPanel->Controls->Add(
                this->lblTitle
            );

            this->actionPanel =
                gcnew System::Windows::Forms::FlowLayoutPanel();

            this->actionPanel->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->actionPanel->Padding =
                System::Windows::Forms::Padding(
                    20,
                    8,
                    20,
                    8
                );

            safe_cast<System::Windows::Forms::FlowLayoutPanel^>(
                this->actionPanel
            )->FlowDirection =
                System::Windows::Forms::FlowDirection::LeftToRight;

            safe_cast<System::Windows::Forms::FlowLayoutPanel^>(
                this->actionPanel
            )->WrapContents = false;

            safe_cast<System::Windows::Forms::FlowLayoutPanel^>(
                this->actionPanel
            )->AutoScroll = true;

            ConfigureButton(this->btnNew, L"New Combination");
            ConfigureButton(this->btnEdit, L"Edit");
            ConfigureButton(this->btnToggle, L"Activate / Deactivate");

            this->btnEdit->Enabled = false;
            this->btnToggle->Enabled = false;

            this->actionPanel->Controls->Add(
                this->btnNew
            );

            this->actionPanel->Controls->Add(
                this->btnEdit
            );

            this->actionPanel->Controls->Add(
                this->btnToggle
            );

            this->grid->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->grid->AllowUserToAddRows = false;
            this->grid->AllowUserToDeleteRows = false;
            this->grid->AllowUserToResizeRows = false;
            this->grid->ReadOnly = true;
            this->grid->MultiSelect = false;
            this->grid->SelectionMode =
                System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->grid->AutoSizeColumnsMode =
                System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
            this->grid->RowHeadersVisible = false;
            this->grid->BackgroundColor =
                System::Drawing::Color::White;
            this->grid->BorderStyle =
                System::Windows::Forms::BorderStyle::None;

            this->grid->ColumnHeadersHeight = 40;
            this->grid->EnableHeadersVisualStyles = false;

            this->grid->ColumnHeadersDefaultCellStyle->BackColor =
                System::Drawing::Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->grid->ColumnHeadersDefaultCellStyle->ForeColor =
                System::Drawing::Color::White;

            this->grid->ColumnHeadersDefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.0F,
                    System::Drawing::FontStyle::Bold
                );

            this->grid->AlternatingRowsDefaultCellStyle->BackColor =
                System::Drawing::Color::FromArgb(
                    248,
                    250,
                    252
                );

            this->grid->Columns->Add(
                L"CombinationId",
                L"ID"
            );

            this->grid->Columns->Add(
                L"CombinationCode",
                L"Code"
            );

            this->grid->Columns->Add(
                L"CombinationName",
                L"Combination"
            );

            this->grid->Columns->Add(
                L"Principals",
                L"Principal Subjects"
            );

            this->grid->Columns->Add(
                L"Subsidiary",
                L"Subsidiary"
            );

            this->grid->Columns->Add(
                L"Status",
                L"Status"
            );

            this->grid->Columns["CombinationId"]->Visible = false;

            this->bottomPanel->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->bottomPanel->FlowDirection =
                System::Windows::Forms::FlowDirection::RightToLeft;

            this->bottomPanel->WrapContents = false;

            this->bottomPanel->Padding =
                System::Windows::Forms::Padding(
                    20,
                    8,
                    20,
                    8
                );

            ConfigureButton(this->btnBack, L"Back to Dashboard");
            this->bottomPanel->Controls->Add(
                this->btnBack
            );

            this->mainLayout->Controls->Add(
                this->headerPanel,
                0,
                0
            );

            this->mainLayout->Controls->Add(
                this->actionPanel,
                0,
                1
            );

            this->mainLayout->Controls->Add(
                this->grid,
                0,
                2
            );

            this->mainLayout->Controls->Add(
                this->bottomPanel,
                0,
                3
            );

            this->Controls->Add(
                this->mainLayout
            );

            this->Name =
                L"CombinationManagement";

            this->ResumeLayout(false);
        }

#pragma endregion
    };
}
