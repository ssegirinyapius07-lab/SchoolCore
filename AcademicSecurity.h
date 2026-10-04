#pragma once

#include "DbConnection.h"
#include "AcademicContext.h"
#include "AuthSession.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <stdexcept>

namespace SchoolCore
{
    public ref class AcademicSecurity abstract sealed
    {
    public:

        static bool IsCurrentActivePeriod(
            int academicYearId,
            int termId)
        {
            return
                AcademicContext::IsAcademicYearActive(
                    academicYearId
                )
                &&
                AcademicContext::IsTermActive(
                    academicYearId,
                    termId
                );
        }

        static bool HasInactiveYearOverridePermission()
        {
            return AuthSession::HasPermission(
                L"academic_records.override_inactive_year"
            );
        }

        static int CreateInactiveYearEditRequest(
            int academicYearId,
            int termId,
            String^ operation,
            String^ tableName,
            String^ recordId,
            String^ reason)
        {
            if (academicYearId <= 0)
            {
                throw gcnew System::ArgumentException(
                    L"Academic year is required."
                );
            }

            if (String::IsNullOrWhiteSpace(reason))
            {
                throw gcnew System::ArgumentException(
                    L"A reason is required for an inactive-year edit."
                );
            }

            if (!HasInactiveYearOverridePermission())
            {
                throw gcnew System::UnauthorizedAccessException(
                    L"You do not have permission to request an inactive-year edit."
                );
            }

            if (AuthSession::UserId <= 0)
            {
                throw gcnew System::UnauthorizedAccessException(
                    L"No authenticated user is available."
                );
            }

            std::string operationText =
                msclr::interop::marshal_as<std::string>(
                    String::IsNullOrWhiteSpace(operation)
                    ? L"UPDATE"
                    : operation
                );

            std::string tableText =
                msclr::interop::marshal_as<std::string>(
                    tableName == nullptr
                    ? L""
                    : tableName
                );

            std::string recordText =
                msclr::interop::marshal_as<std::string>(
                    recordId == nullptr
                    ? L""
                    : recordId
                );

            std::string reasonText =
                msclr::interop::marshal_as<std::string>(
                    reason->Trim()
                );

            auto con =
                DbConnection::GetConnection();

            std::unique_ptr<sql::PreparedStatement> stmt(
                con->prepareStatement(
                    "INSERT INTO academic_edit_approvals "
                    "("
                    "academic_year_id, "
                    "term_id, "
                    "operation, "
                    "table_name, "
                    "record_id, "
                    "reason, "
                    "requested_by, "
                    "status"
                    ") "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, 'Pending')"
                )
            );

            stmt->setInt(
                1,
                academicYearId
            );

            if (termId > 0)
                stmt->setInt(2, termId);
            else
                stmt->setNull(
                    2,
                    sql::DataType::INTEGER
                );

            stmt->setString(
                3,
                operationText
            );

            stmt->setString(
                4,
                tableText
            );

            stmt->setString(
                5,
                recordText
            );

            stmt->setString(
                6,
                reasonText
            );

            stmt->setInt(
                7,
                AuthSession::UserId
            );

            stmt->executeUpdate();

            std::unique_ptr<sql::Statement> idStmt(
                con->createStatement()
            );

            std::unique_ptr<sql::ResultSet> idResult(
                idStmt->executeQuery(
                    "SELECT LAST_INSERT_ID() AS approval_id"
                )
            );

            if (!idResult->next())
            {
                throw std::runtime_error(
                    "Unable to retrieve the approval request ID."
                );
            }

            return idResult->getInt(
                "approval_id"
            );
        }

        static bool HasApprovedOverride(
            int approvalId,
            int academicYearId,
            int termId,
            String^ operation,
            String^ tableName,
            String^ recordId)
        {
            if (approvalId <= 0 ||
                academicYearId <= 0 ||
                AuthSession::UserId <= 0)
            {
                return false;
            }

            std::string operationText =
                msclr::interop::marshal_as<std::string>(
                    operation == nullptr
                    ? L"UPDATE"
                    : operation
                );

            std::string tableText =
                msclr::interop::marshal_as<std::string>(
                    tableName == nullptr
                    ? L""
                    : tableName
                );

            std::string recordText =
                msclr::interop::marshal_as<std::string>(
                    recordId == nullptr
                    ? L""
                    : recordId
                );

            auto con =
                DbConnection::GetConnection();

            std::unique_ptr<sql::PreparedStatement> stmt(
                con->prepareStatement(
                    "SELECT approval_id "
                    "FROM academic_edit_approvals "
                    "WHERE approval_id = ? "
                    "AND academic_year_id = ? "
                    "AND (term_id = ? OR (? = 0 AND term_id IS NULL)) "
                    "AND operation = ? "
                    "AND table_name = ? "
                    "AND COALESCE(record_id, '') = ? "
                    "AND requested_by = ? "
                    "AND status = 'Approved' "
                    "AND approved_by IS NOT NULL "
                    "AND approved_by <> requested_by "
                    "LIMIT 1"
                )
            );

            stmt->setInt(
                1,
                approvalId
            );

            stmt->setInt(
                2,
                academicYearId
            );

            if (termId > 0)
                stmt->setInt(3, termId);
            else
                stmt->setNull(
                    3,
                    sql::DataType::INTEGER
                );

            stmt->setInt(
                4,
                termId
            );

            stmt->setString(
                5,
                operationText
            );

            stmt->setString(
                6,
                tableText
            );

            stmt->setString(
                7,
                recordText
            );

            stmt->setInt(
                8,
                AuthSession::UserId
            );

            std::unique_ptr<sql::ResultSet> result(
                stmt->executeQuery()
            );

            return result->next();
        }

        static void WriteAudit(
            String^ action,
            String^ tableName,
            String^ recordId,
            String^ description,
            String^ reason,
            long long approvalId,
            String^ oldValues,
            String^ newValues)
        {
            auto con =
                DbConnection::GetConnection();

            std::unique_ptr<sql::PreparedStatement> stmt(
                con->prepareStatement(
                    "INSERT INTO audit_logs "
                    "("
                    "user_id, "
                    "action, "
                    "table_name, "
                    "record_id, "
                    "description, "
                    "reason, "
                    "approval_id, "
                    "old_values, "
                    "new_values"
                    ") "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
                )
            );

            if (AuthSession::UserId > 0)
                stmt->setInt(
                    1,
                    AuthSession::UserId
                );
            else
                stmt->setNull(
                    1,
                    sql::DataType::INTEGER
                );

            stmt->setString(
                2,
                msclr::interop::marshal_as<std::string>(
                    action == nullptr ? L"" : action
                )
            );

            stmt->setString(
                3,
                msclr::interop::marshal_as<std::string>(
                    tableName == nullptr ? L"" : tableName
                )
            );

            stmt->setString(
                4,
                msclr::interop::marshal_as<std::string>(
                    recordId == nullptr ? L"" : recordId
                )
            );

            stmt->setString(
                5,
                msclr::interop::marshal_as<std::string>(
                    description == nullptr ? L"" : description
                )
            );

            if (String::IsNullOrWhiteSpace(reason))
                stmt->setNull(
                    6,
                    sql::DataType::VARCHAR
                );
            else
                stmt->setString(
                    6,
                    msclr::interop::marshal_as<std::string>(
                        reason
                    )
                );

            if (approvalId > 0)
                stmt->setInt64(
                    7,
                    approvalId
                );
            else
                stmt->setNull(
                    7,
                    sql::DataType::BIGINT
                );

            if (String::IsNullOrWhiteSpace(oldValues))
                stmt->setNull(
                    8,
                    sql::DataType::LONGVARCHAR
                );
            else
                stmt->setString(
                    8,
                    msclr::interop::marshal_as<std::string>(
                        oldValues
                    )
                );

            if (String::IsNullOrWhiteSpace(newValues))
                stmt->setNull(
                    9,
                    sql::DataType::LONGVARCHAR
                );
            else
                stmt->setString(
                    9,
                    msclr::interop::marshal_as<std::string>(
                        newValues
                    )
                );

            stmt->executeUpdate();
        }
    };
}
