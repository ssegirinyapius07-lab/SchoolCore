#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <memory>
#include <string>
#include <msclr/marshal_cppstd.h>

using namespace System;

namespace SchoolCore
{
    public ref class GradingEngine abstract sealed
    {
    private:

        static String^ GetString(
            sql::ResultSet* result,
            const char* column)
        {
            if (result->isNull(column))
                return L"";

            return gcnew String(
                result->getString(column).c_str()
            );
        }

        static void UpsertPendingResult(
            sql::Connection* con,
            int enrollmentId,
            int examinationSubjectId,
            double totalScore,
            double totalMaxScore,
            double percentage,
            int activePaperCount,
            int scoredPaperCount)
        {
            std::unique_ptr<sql::PreparedStatement> stmt(
                con->prepareStatement(
                    "INSERT INTO student_subject_results "
                    "(enrollment_id, examination_subject_id, "
                    "total_score, total_max_score, percentage, "
                    "grade, grade_point, grade_weight, remarks, "
                    "paper_count, status, calculated_at) "
                    "VALUES (?, ?, ?, ?, ?, NULL, NULL, NULL, ?, ?, 'Pending', CURRENT_TIMESTAMP) "
                    "ON DUPLICATE KEY UPDATE "
                    "total_score = VALUES(total_score), "
                    "total_max_score = VALUES(total_max_score), "
                    "percentage = VALUES(percentage), "
                    "grade = NULL, "
                    "grade_point = NULL, "
                    "grade_weight = NULL, "
                    "remarks = VALUES(remarks), "
                    "paper_count = VALUES(paper_count), "
                    "status = 'Pending', "
                    "calculated_at = CURRENT_TIMESTAMP"
                )
            );

            stmt->setInt(1, enrollmentId);
            stmt->setInt(2, examinationSubjectId);
            stmt->setDouble(3, totalScore);
            stmt->setDouble(4, totalMaxScore);
            stmt->setDouble(5, percentage);
            stmt->setString(
                6,
                "Pending: " +
                std::to_string(scoredPaperCount) +
                "/" +
                std::to_string(activePaperCount) +
                " assigned papers have marks."
            );
            stmt->setInt(7, activePaperCount);
            stmt->executeUpdate();
        }

    public:

        // Calculates the subject result for an examination.
        //
        // Important rules:
        //   1. Every active paper assigned to the examination subject is required.
        //   2. Scores are combined using each paper's real maximum score.
        //   3. An incomplete subject never receives a final grade, point or weight.
        //   4. The grading policy is curriculum-aware:
        //        - UACE aligned (2025+): A-E + 5..1 performance weight, no legacy 6..0 points.
        //        - UACE legacy: A=6 ... F=0 points.
        //        - UCE CBC: provisional A-E policy.
        //        - School custom: database-configurable policy.
        static bool RecalculateSubjectResult(
            int examinationSubjectId,
            int studentId)
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                int academicYearId = 0;
                int termId = 0;
                int classId = 0;
                int enrollmentId = 0;
                int academicLevelId = 0;
                int curriculumId = 0;
                int gradingPolicyId = 0;

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "e.academic_year_id, "
                            "e.term_id, "
                            "e.class_id, "
                            "c.academic_level_id, "
                            "(SELECT cur2.curriculum_id "
                            " FROM curricula cur2 "
                            " WHERE cur2.academic_level_id = c.academic_level_id "
                            " AND cur2.status = 'Active' "
                            " ORDER BY cur2.effective_from_year DESC, cur2.curriculum_id DESC "
                            " LIMIT 1) AS resolved_curriculum_id, "
                            "COALESCE(e.grading_policy_id, "
                            "  (SELECT gp.policy_id "
                            "   FROM grading_policies gp "
                            "   WHERE gp.status = 'Active' "
                            "   AND gp.academic_level_id = c.academic_level_id "
                            "   AND gp.curriculum_id = "
                            "       (SELECT cur3.curriculum_id "
                            "        FROM curricula cur3 "
                            "        WHERE cur3.academic_level_id = c.academic_level_id "
                            "        AND cur3.status = 'Active' "
                            "        ORDER BY cur3.effective_from_year DESC, cur3.curriculum_id DESC "
                            "        LIMIT 1) "
                            "   ORDER BY gp.effective_from_year DESC, gp.policy_id DESC "
                            "   LIMIT 1) "
                            ") AS resolved_policy_id "
                            "FROM examination_subjects es "
                            "INNER JOIN examinations e "
                            "ON e.examination_id = es.examination_id "
                            "INNER JOIN classes c "
                            "ON c.class_id = e.class_id "
                            "WHERE es.examination_subject_id = ?"
                        )
                    );

                    stmt->setInt(1, examinationSubjectId);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (!result->next())
                        return false;

                    academicYearId =
                        result->getInt("academic_year_id");

                    termId =
                        result->getInt("term_id");

                    classId =
                        result->getInt("class_id");

                    academicLevelId =
                        result->getInt("academic_level_id");

                    curriculumId =
                        result->getInt("resolved_curriculum_id");

                    gradingPolicyId =
                        result->getInt("resolved_policy_id");
                }

                if (
                    academicYearId <= 0 ||
                    termId <= 0 ||
                    classId <= 0 ||
                    academicLevelId <= 0 ||
                    curriculumId <= 0 ||
                    gradingPolicyId <= 0)
                {
                    return false;
                }

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT enrollment_id "
                            "FROM enrollments "
                            "WHERE student_id = ? "
                            "AND academic_year_id = ? "
                            "AND term_id = ? "
                            "AND class_id = ? "
                            "AND status = 'Active' "
                            "ORDER BY enrollment_id DESC "
                            "LIMIT 1"
                        )
                    );

                    stmt->setInt(1, studentId);
                    stmt->setInt(2, academicYearId);
                    stmt->setInt(3, termId);
                    stmt->setInt(4, classId);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (!result->next())
                        return false;

                    enrollmentId =
                        result->getInt("enrollment_id");
                }

                int activePaperCount = 0;
                int scoredPaperCount = 0;
                double totalScore = 0.0;
                double totalMaxScore = 0.0;

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "COUNT(*) AS active_papers, "
                            "COALESCE(SUM(ep.max_score), 0) AS total_max_score, "
                            "COALESCE(SUM(CASE "
                            "WHEN m.mark_id IS NOT NULL THEN 1 "
                            "ELSE 0 END), 0) AS scored_papers, "
                            "COALESCE(SUM(COALESCE(m.score, 0)), 0) AS total_score "
                            "FROM examination_papers ep "
                            "LEFT JOIN marks m "
                            "ON m.examination_paper_id = ep.examination_paper_id "
                            "AND m.student_id = ? "
                            "WHERE ep.examination_subject_id = ? "
                            "AND ep.status = 'Active'"
                        )
                    );

                    stmt->setInt(1, studentId);
                    stmt->setInt(2, examinationSubjectId);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (!result->next())
                        return false;

                    activePaperCount =
                        result->getInt("active_papers");

                    scoredPaperCount =
                        result->getInt("scored_papers");

                    totalScore =
                        result->getDouble("total_score");

                    totalMaxScore =
                        result->getDouble("total_max_score");
                }

                if (
                    activePaperCount == 0 ||
                    totalMaxScore <= 0.0)
                {
                    return false;
                }

                double percentage =
                    (totalScore / totalMaxScore) * 100.0;

                // Do not produce a final subject grade until every assigned
                // paper has a mark for this student.
                if (scoredPaperCount != activePaperCount)
                {
                    UpsertPendingResult(
                        con.get(),
                        enrollmentId,
                        examinationSubjectId,
                        totalScore,
                        totalMaxScore,
                        percentage,
                        activePaperCount,
                        scoredPaperCount
                    );

                    return false;
                }

                String^ calculationMethod = L"";
                String^ grade = L"";
                String^ remarks = L"";
                double gradePoint = 0.0;
                double gradeWeight = 0.0;
                bool hasGradePoint = false;
                bool hasGradeWeight = false;

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "gp.calculation_method, "
                            "b.grade, "
                            "b.grade_point, "
                            "b.grade_weight, "
                            "b.remarks "
                            "FROM grading_policy_bands b "
                            "INNER JOIN grading_policies gp "
                            "ON gp.policy_id = b.policy_id "
                            "WHERE b.policy_id = ? "
                            "AND b.status = 'Active' "
                            "AND ? BETWEEN b.min_value AND b.max_value "
                            "AND gp.status = 'Active' "
                            "ORDER BY b.band_order ASC "
                            "LIMIT 1"
                        )
                    );

                    stmt->setInt(1, gradingPolicyId);
                    stmt->setDouble(2, percentage);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (!result->next())
                        return false;

                    calculationMethod =
                        GetString(
                            result.get(),
                            "calculation_method"
                        );

                    grade =
                        GetString(
                            result.get(),
                            "grade"
                        );

                    if (!result->isNull("grade_point"))
                    {
                        gradePoint =
                            result->getDouble("grade_point");

                        hasGradePoint = true;
                    }

                    if (!result->isNull("grade_weight"))
                    {
                        gradeWeight =
                            result->getDouble("grade_weight");

                        hasGradeWeight = true;
                    }

                    remarks =
                        GetString(
                            result.get(),
                            "remarks"
                        );
                }

                // Current UACE aligned curriculum is A-E with 5..1 performance
                // weights. The historical A=6...F=0 points are deliberately
                // unavailable unless the legacy policy is selected.
                if (
                    curriculumId == 2 &&
                    calculationMethod == L"PAPER_AVERAGE_AE")
                {
                    hasGradePoint = false;
                }

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "INSERT INTO student_subject_results "
                            "(enrollment_id, examination_subject_id, "
                            "total_score, total_max_score, percentage, "
                            "grade, grade_point, grade_weight, remarks, "
                            "paper_count, status, calculated_at) "
                            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 'Calculated', CURRENT_TIMESTAMP) "
                            "ON DUPLICATE KEY UPDATE "
                            "total_score = VALUES(total_score), "
                            "total_max_score = VALUES(total_max_score), "
                            "percentage = VALUES(percentage), "
                            "grade = VALUES(grade), "
                            "grade_point = VALUES(grade_point), "
                            "grade_weight = VALUES(grade_weight), "
                            "remarks = VALUES(remarks), "
                            "paper_count = VALUES(paper_count), "
                            "status = 'Calculated', "
                            "calculated_at = CURRENT_TIMESTAMP"
                        )
                    );

                    stmt->setInt(1, enrollmentId);
                    stmt->setInt(2, examinationSubjectId);
                    stmt->setDouble(3, totalScore);
                    stmt->setDouble(4, totalMaxScore);
                    stmt->setDouble(5, percentage);

                    if (String::IsNullOrWhiteSpace(grade))
                        stmt->setNull(6, sql::DataType::VARCHAR);
                    else
                        stmt->setString(
                            6,
                            msclr::interop::marshal_as<std::string>(
                                grade
                            )
                        );

                    if (hasGradePoint)
                        stmt->setDouble(7, gradePoint);
                    else
                        stmt->setNull(7, sql::DataType::DECIMAL);

                    if (hasGradeWeight)
                        stmt->setDouble(8, gradeWeight);
                    else
                        stmt->setNull(8, sql::DataType::DECIMAL);

                    if (String::IsNullOrWhiteSpace(remarks))
                        stmt->setNull(9, sql::DataType::VARCHAR);
                    else
                        stmt->setString(
                            9,
                            msclr::interop::marshal_as<std::string>(
                                remarks
                            )
                        );

                    stmt->setInt(10, activePaperCount);

                    stmt->executeUpdate();
                }

                return true;
            }
            catch (sql::SQLException&)
            {
                return false;
            }
        }
    };
}
