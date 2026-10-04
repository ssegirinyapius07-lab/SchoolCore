#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <memory>
#include <string>

namespace SchoolCore
{
    public ref class AcademicYearInfo
    {
    public:
        int Id;
        System::String^ Name;

        AcademicYearInfo(
            int id,
            System::String^ name)
        {
            Id = id;
            Name = name;
        }
    };

    public ref class AcademicContext abstract sealed
    {
    public:
        static AcademicYearInfo^ GetActiveAcademicYear()
        {
            auto con = DbConnection::GetConnection();

            std::unique_ptr<sql::PreparedStatement> stmt(
                con->prepareStatement(
                    "SELECT "
                    "academic_year_id, "
                    "year_name "
                    "FROM academic_years "
                    "WHERE status = 'Active' "
                    "ORDER BY "
                    "start_date DESC, "
                    "academic_year_id DESC "
                    "LIMIT 1"
                )
            );

            std::unique_ptr<sql::ResultSet> result(
                stmt->executeQuery()
            );

            if (!result->next())
            {
                throw std::runtime_error(
                    "No active academic year is configured."
                );
            }

            return gcnew AcademicYearInfo(
                result->getInt("academic_year_id"),
                gcnew System::String(
                    result->getString("year_name").c_str()
                )
            );
        }

        static int GetActiveAcademicYearId()
        {
            AcademicYearInfo^ year =
                GetActiveAcademicYear();

            return year->Id;
        }

        static System::String^ GetActiveAcademicYearName()
        {
            AcademicYearInfo^ year =
                GetActiveAcademicYear();

            return year->Name;
        }

        static int GetActiveTermId()
        {
            auto con = DbConnection::GetConnection();

            std::unique_ptr<sql::PreparedStatement> stmt(
                con->prepareStatement(
                    "SELECT "
                    "t.term_id "
                    "FROM terms t "
                    "INNER JOIN academic_years ay "
                    "ON ay.academic_year_id = t.academic_year_id "
                    "WHERE ay.status = 'Active' "
                    "AND t.status = 'Active' "
                    "ORDER BY t.term_id "
                    "LIMIT 1"
                )
            );

            std::unique_ptr<sql::ResultSet> result(
                stmt->executeQuery()
            );

            if (!result->next())
            {
                throw std::runtime_error(
                    "No active term is configured for the active academic year."
                );
            }

            return result->getInt("term_id");
        }
    };
}
