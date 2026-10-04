-- SchoolCore: curriculum-aware assessment and grading policies.
-- Current policy architecture:
--   * UCE_CBC_PROVISIONAL: school-configurable A-E scale for the new UCE/CBC level.
--   * UACE_ALIGNED_PROVISIONAL: paper-balanced internal subject result using A-E and
--     5..1 performance weights. This is a SchoolCore implementation of the
--     current aligned A-Level philosophy; the official end-of-cycle result is
--     construct-based and subject-specific.
--   * UACE_LEGACY_20_POINT: historical A=6 ... F=0 policy for legacy cohorts.
--   * SCHOOL_CUSTOM_PERCENTAGE: reusable school-defined percentage bands.
--
-- The national aligned Advanced Secondary framework uses A-E final grades and
-- 5..1 grade weights at construct level. It does not use the historical 20-point
-- A=6...F=0 method for the aligned curriculum.

CREATE TABLE IF NOT EXISTS grading_policies (
    policy_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    curriculum_id INT(10) UNSIGNED DEFAULT NULL,
    academic_level_id INT(10) UNSIGNED NOT NULL,
    policy_code VARCHAR(80) NOT NULL,
    policy_name VARCHAR(150) NOT NULL,
    policy_type VARCHAR(40) NOT NULL DEFAULT 'School',
    calculation_method VARCHAR(60) NOT NULL DEFAULT 'PERCENTAGE',
    effective_from_year INT(11) DEFAULT NULL,
    effective_to_year INT(11) DEFAULT NULL,
    description VARCHAR(500) DEFAULT NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (policy_id),
    UNIQUE KEY uq_grading_policy_code (policy_code),
    KEY idx_grading_policy_curriculum (curriculum_id),
    KEY idx_grading_policy_level (academic_level_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS grading_policy_bands (
    policy_band_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    policy_id INT(10) UNSIGNED NOT NULL,
    band_order INT(10) UNSIGNED NOT NULL,
    min_value DECIMAL(8,3) NOT NULL,
    max_value DECIMAL(8,3) NOT NULL,
    grade VARCHAR(20) NOT NULL,
    grade_weight DECIMAL(6,2) DEFAULT NULL,
    grade_point DECIMAL(6,2) DEFAULT NULL,
    remarks VARCHAR(255) DEFAULT NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (policy_band_id),
    UNIQUE KEY uq_grading_policy_band (policy_id, band_order),
    KEY idx_grading_policy_band_range (policy_id, min_value, max_value),
    CONSTRAINT fk_grading_policy_band_policy
        FOREIGN KEY (policy_id)
        REFERENCES grading_policies (policy_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- An examination may explicitly choose a grading policy. When NULL, SchoolCore
-- resolves the current policy from the class academic level/curriculum.
SET @has_grading_policy_id := (
    SELECT COUNT(*)
    FROM information_schema.COLUMNS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'examinations'
      AND COLUMN_NAME = 'grading_policy_id'
);

SET @sql := IF(
    @has_grading_policy_id = 0,
    'ALTER TABLE examinations ADD COLUMN grading_policy_id INT(10) UNSIGNED NULL AFTER examination_type',
    'SELECT 1'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- Stores the new curriculum's 5..1 performance weight without pretending that
-- it is the historical 6..0 UACE grade-point system.
SET @has_grade_weight := (
    SELECT COUNT(*)
    FROM information_schema.COLUMNS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'student_subject_results'
      AND COLUMN_NAME = 'grade_weight'
);

SET @sql := IF(
    @has_grade_weight = 0,
    'ALTER TABLE student_subject_results ADD COLUMN grade_weight DECIMAL(6,2) NULL AFTER grade_point',
    'SELECT 1'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- Preserve a legacy curriculum record so old cohorts can be represented
-- without confusing them with the current aligned A-Level curriculum.
INSERT INTO curricula
    (curriculum_code, curriculum_name, academic_level_id,
     effective_from_year, effective_to_year, description, status)
SELECT
    'UACE_LEGACY',
    'Legacy UACE Curriculum',
    al.academic_level_id,
    NULL,
    2025,
    'Historical UACE curriculum for cohorts assessed under the former A-Level structure.',
    'Legacy'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM curricula c
      WHERE c.curriculum_code = 'UACE_LEGACY'
  );

-- Default policies.
INSERT INTO grading_policies
    (curriculum_id, academic_level_id, policy_code, policy_name,
     policy_type, calculation_method, effective_from_year, description, status)
SELECT
    c.curriculum_id,
    al.academic_level_id,
    'UCE_CBC_PROVISIONAL',
    'UCE/CBC Provisional A-E',
    'NationalAlignedProvisional',
    'PERCENTAGE_AE',
    2026,
    'Provisional SchoolCore percentage implementation for the new UCE/CBC level. Configure or replace as official UNEB detail is refined.',
    'Active'
FROM curricula c
INNER JOIN academic_levels al
    ON al.level_code = 'O_LEVEL'
WHERE c.curriculum_code = 'UCE_CBC'
  AND NOT EXISTS (
      SELECT 1 FROM grading_policies gp
      WHERE gp.policy_code = 'UCE_CBC_PROVISIONAL'
  );

INSERT INTO grading_policies
    (curriculum_id, academic_level_id, policy_code, policy_name,
     policy_type, calculation_method, effective_from_year, description, status)
SELECT
    c.curriculum_id,
    al.academic_level_id,
    'UACE_ALIGNED_PROVISIONAL',
    'UACE Aligned 2025+ A-E',
    'NationalAlignedProvisional',
    'PAPER_AVERAGE_AE',
    2025,
    'SchoolCore implementation for aligned Advanced Secondary subject results. Final national certification is construct-based; this school engine combines all assigned papers using actual maximum scores and reports A-E with 5..1 performance weights.',
    'Active'
FROM curricula c
INNER JOIN academic_levels al
    ON al.level_code = 'A_LEVEL'
WHERE c.curriculum_code = 'UACE_ALIGNED'
  AND NOT EXISTS (
      SELECT 1 FROM grading_policies gp
      WHERE gp.policy_code = 'UACE_ALIGNED_PROVISIONAL'
  );

INSERT INTO grading_policies
    (curriculum_id, academic_level_id, policy_code, policy_name,
     policy_type, calculation_method, effective_to_year, description, status)
SELECT
    c.curriculum_id,
    al.academic_level_id,
    'UACE_LEGACY_20_POINT',
    'Legacy UACE A=6 to F=0',
    'LegacyNational',
    'PERCENTAGE_LEGACY_20',
    2025,
    'Historical UACE percentage-to-grade and A=6...F=0 point scale. Not used by the aligned 2025+ curriculum.',
    'Active'
FROM curricula c
INNER JOIN academic_levels al
    ON al.level_code = 'A_LEVEL'
WHERE c.curriculum_code = 'UACE_LEGACY'
  AND NOT EXISTS (
      SELECT 1 FROM grading_policies gp
      WHERE gp.policy_code = 'UACE_LEGACY_20_POINT'
  );

INSERT INTO grading_policies
    (curriculum_id, academic_level_id, policy_code, policy_name,
     policy_type, calculation_method, description, status)
SELECT
    NULL,
    al.academic_level_id,
    CONCAT('SCHOOL_CUSTOM_', al.level_code),
    CONCAT('School Custom ', al.level_name),
    'SchoolCustom',
    'PERCENTAGE_CUSTOM',
    'Reusable school-defined percentage grading policy. Schools may define their own bands without changing application code.',
    'Active'
FROM academic_levels al
WHERE NOT EXISTS (
    SELECT 1 FROM grading_policies gp
    WHERE gp.policy_code = CONCAT('SCHOOL_CUSTOM_', al.level_code)
);

-- UCE provisional A-E.
INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 1, 80.00, 100.00, 'A', 5.00, 4.00, 'Exceptional'
FROM grading_policies gp
WHERE gp.policy_code = 'UCE_CBC_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 1);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 2, 70.00, 79.99, 'B', 4.00, 3.00, 'Outstanding'
FROM grading_policies gp
WHERE gp.policy_code = 'UCE_CBC_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 2);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 3, 60.00, 69.99, 'C', 3.00, 2.00, 'Satisfactory'
FROM grading_policies gp
WHERE gp.policy_code = 'UCE_CBC_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 3);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 4, 50.00, 59.99, 'D', 2.00, 1.00, 'Basic'
FROM grading_policies gp
WHERE gp.policy_code = 'UCE_CBC_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 4);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 5, 0.00, 49.99, 'E', 1.00, 0.00, 'Elementary'
FROM grading_policies gp
WHERE gp.policy_code = 'UCE_CBC_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 5);

-- Aligned Advanced Secondary provisional paper-combined A-E.
-- The grade weight 5..1 corresponds to the NCDC framework's construct weights.
INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 1, 80.00, 100.00, 'A', 5.00, NULL, 'Exceptional'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_ALIGNED_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 1);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 2, 70.00, 79.99, 'B', 4.00, NULL, 'Outstanding'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_ALIGNED_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 2);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 3, 60.00, 69.99, 'C', 3.00, NULL, 'Satisfactory'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_ALIGNED_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 3);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 4, 50.00, 59.99, 'D', 2.00, NULL, 'Basic'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_ALIGNED_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 4);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 5, 0.00, 49.99, 'E', 1.00, NULL, 'Elementary'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_ALIGNED_PROVISIONAL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 5);

-- Legacy A=6 ... F=0.
INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 1, 80.00, 100.00, 'A', 5.00, 6.00, 'Excellent'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_LEGACY_20_POINT'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 1);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 2, 70.00, 79.99, 'B', 4.00, 5.00, 'Very Good'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_LEGACY_20_POINT'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 2);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 3, 60.00, 69.99, 'C', 3.00, 4.00, 'Good'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_LEGACY_20_POINT'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 3);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 4, 50.00, 59.99, 'D', 2.00, 3.00, 'Satisfactory'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_LEGACY_20_POINT'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 4);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 5, 40.00, 49.99, 'E', 1.00, 2.00, 'Pass'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_LEGACY_20_POINT'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 5);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 6, 30.00, 39.99, 'O', 0.00, 1.00, 'Ordinary'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_LEGACY_20_POINT'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 6);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 7, 0.00, 29.99, 'F', 0.00, 0.00, 'Fail'
FROM grading_policies gp
WHERE gp.policy_code = 'UACE_LEGACY_20_POINT'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 7);

-- Starter custom bands for both levels. These are deliberately school-policy
-- defaults, not UNEB claims, and may be edited later.
INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 1, 80.00, 100.00, 'A', 5.00, 4.00, 'School-defined Exceptional'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_O_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 1);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 2, 70.00, 79.99, 'B', 4.00, 3.00, 'School-defined Outstanding'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_O_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 2);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 3, 60.00, 69.99, 'C', 3.00, 2.00, 'School-defined Satisfactory'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_O_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 3);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 4, 50.00, 59.99, 'D', 2.00, 1.00, 'School-defined Basic'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_O_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 4);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 5, 0.00, 49.99, 'E', 1.00, 0.00, 'School-defined Elementary'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_O_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 5);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 1, 80.00, 100.00, 'A', 5.00, NULL, 'School-defined Exceptional'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_A_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 1);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 2, 70.00, 79.99, 'B', 4.00, NULL, 'School-defined Outstanding'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_A_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 2);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 3, 60.00, 69.99, 'C', 3.00, NULL, 'School-defined Satisfactory'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_A_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 3);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 4, 50.00, 59.99, 'D', 2.00, NULL, 'School-defined Basic'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_A_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 4);

INSERT INTO grading_policy_bands
    (policy_id, band_order, min_value, max_value, grade, grade_weight, grade_point, remarks)
SELECT gp.policy_id, 5, 0.00, 49.99, 'E', 1.00, NULL, 'School-defined Elementary'
FROM grading_policies gp
WHERE gp.policy_code = 'SCHOOL_CUSTOM_A_LEVEL'
  AND NOT EXISTS (SELECT 1 FROM grading_policy_bands b WHERE b.policy_id = gp.policy_id AND b.band_order = 5);

-- Seed current aligned A-Level paper catalogue for subjects already present in SchoolCore.
-- The 2026 aligned framework uses two end-of-cycle papers for these subjects.
-- Paper codes are taken from 2026 UNEB sample papers/materials.
SET @uace_curriculum_id := (
    SELECT curriculum_id FROM curricula
    WHERE curriculum_code = 'UACE_ALIGNED'
    LIMIT 1
);

-- Normalize the original A-Level Mathematics seed rows BEFORE adding
-- the new aligned paper catalogue. This ordering prevents a duplicate
-- curriculum_subject_id + paper_code when a clean database contains P425/245.
SET @uace_curriculum_id := (
    SELECT curriculum_id FROM curricula
    WHERE curriculum_code = 'UACE_ALIGNED'
    LIMIT 1
);

UPDATE subject_papers sp
INNER JOIN curriculum_subjects cs
    ON cs.curriculum_subject_id = sp.curriculum_subject_id
SET
    sp.paper_code = 'P425/1',
    sp.paper_number = '1',
    sp.paper_name = 'Principal Mathematics Paper 1',
    sp.paper_type = 'Theory',
    sp.status = 'Active'
WHERE sp.subject_id = 1
  AND sp.academic_level_id = 2
  AND cs.curriculum_id = @uace_curriculum_id
  AND sp.paper_code = 'P425'
  AND NOT EXISTS (
      SELECT 1
      FROM subject_papers existing_p1
      WHERE existing_p1.curriculum_subject_id = sp.curriculum_subject_id
        AND existing_p1.paper_code = 'P425/1'
  );

UPDATE subject_papers sp
INNER JOIN curriculum_subjects cs
    ON cs.curriculum_subject_id = sp.curriculum_subject_id
SET
    sp.paper_code = 'P425/2',
    sp.paper_number = '2',
    sp.paper_name = 'Principal Mathematics Paper 2',
    sp.paper_type = 'Theory',
    sp.status = 'Active'
WHERE sp.subject_id = 1
  AND sp.academic_level_id = 2
  AND cs.curriculum_id = @uace_curriculum_id
  AND sp.paper_code = '245'
  AND NOT EXISTS (
      SELECT 1
      FROM subject_papers existing_p2
      WHERE existing_p2.curriculum_subject_id = sp.curriculum_subject_id
        AND existing_p2.paper_code = 'P425/2'
  );

-- Curriculum subject mappings.
INSERT INTO curriculum_subjects
    (curriculum_id, subject_id, uneb_subject_code, subject_group, requirement_type, status)
SELECT @uace_curriculum_id, s.subject_id, x.uneb_code, x.subject_group, 'Optional', 'Active'
FROM subjects s
INNER JOIN (
    SELECT 1 subject_id, 'P425' uneb_code, 'Sciences' subject_group
    UNION ALL SELECT 2, 'P510', 'Sciences'
    UNION ALL SELECT 4, 'P530', 'Sciences'
    UNION ALL SELECT 8, 'P525', 'Sciences'
    UNION ALL SELECT 3, 'P220', 'Arts'
    UNION ALL SELECT 6, 'P250', 'Arts'
    UNION ALL SELECT 10, 'P210', 'Arts'
    UNION ALL SELECT 7, 'P310', 'Arts'
) x ON x.subject_id = s.subject_id
WHERE @uace_curriculum_id IS NOT NULL
  AND NOT EXISTS (
      SELECT 1 FROM curriculum_subjects cs
      WHERE cs.curriculum_id = @uace_curriculum_id
        AND cs.subject_id = s.subject_id
  );

-- Idempotent aligned A-Level paper catalogue.
-- Existing rows are updated in place. This is safe to resume after a
-- partially applied migration and preserves paper IDs referenced by marks.

INSERT INTO subject_papers
    (curriculum_subject_id, subject_id, academic_level_id, paper_code,
     paper_number, paper_name, paper_type, status)
SELECT
    cs.curriculum_subject_id,
    cs.subject_id,
    2,
    p.paper_code,
    p.paper_number,
    p.paper_name,
    p.paper_type,
    'Active'
FROM curriculum_subjects cs
INNER JOIN (
    SELECT 1 subject_id, 'P425/1' paper_code, '1' paper_number,
           'Principal Mathematics Paper 1' paper_name, 'Theory' paper_type
    UNION ALL SELECT 1, 'P425/2', '2',
           'Principal Mathematics Paper 2', 'Theory'
    UNION ALL SELECT 2, 'P510/1', '1',
           'Physics Paper 1', 'Theory'
    UNION ALL SELECT 2, 'P510/2', '2',
           'Physics Paper 2', 'Practical'
    UNION ALL SELECT 4, 'P530/1', '1',
           'Biology Paper 1', 'Theory'
    UNION ALL SELECT 4, 'P530/2', '2',
           'Biology Paper 2', 'Practical'
    UNION ALL SELECT 8, 'P525/1', '1',
           'Chemistry Paper 1', 'Theory'
    UNION ALL SELECT 8, 'P525/2', '2',
           'Chemistry Paper 2', 'Practical'
    UNION ALL SELECT 3, 'P220/1', '1',
           'Economics Paper 1', 'Theory'
    UNION ALL SELECT 3, 'P220/2', '2',
           'Economics Paper 2', 'Theory'
    UNION ALL SELECT 6, 'P250/1', '1',
           'Geography Paper 1', 'Theory'
    UNION ALL SELECT 6, 'P250/2', '2',
           'Geography Paper 2', 'Theory'
    UNION ALL SELECT 10, 'P210/1', '1',
           'History Paper 1', 'Theory'
    UNION ALL SELECT 10, 'P210/2', '2',
           'History Paper 2', 'Theory'
    UNION ALL SELECT 7, 'P310/1', '1',
           'Literature in English Paper 1', 'Theory'
    UNION ALL SELECT 7, 'P310/2', '2',
           'Literature in English Paper 2', 'Theory'
) p ON p.subject_id = cs.subject_id
WHERE cs.curriculum_id = @uace_curriculum_id
  AND cs.status = 'Active'
ON DUPLICATE KEY UPDATE
    subject_id = VALUES(subject_id),
    academic_level_id = VALUES(academic_level_id),
    paper_number = VALUES(paper_number),
    paper_name = VALUES(paper_name),
    paper_type = VALUES(paper_type),
    status = 'Active';

-- If the original malformed seed rows P425/245 still exist alongside the
-- canonical P425/1/P425/2 rows, keep their IDs (and any marks) but move them
-- out of the active curriculum catalogue. Existing examination_papers that
-- reference them remain valid and can still be migrated/closed later.
UPDATE subject_papers sp
INNER JOIN curriculum_subjects cs
    ON cs.curriculum_subject_id = sp.curriculum_subject_id
SET
    sp.paper_code = CONCAT('LEGACY-P425-', sp.paper_id),
    sp.status = 'Inactive'
WHERE sp.subject_id = 1
  AND sp.academic_level_id = 2
  AND cs.curriculum_id = @uace_curriculum_id
  AND sp.paper_code = 'P425'
  AND EXISTS (
      SELECT 1
      FROM subject_papers canon
      WHERE canon.curriculum_subject_id = sp.curriculum_subject_id
        AND canon.paper_code = 'P425/1'
        AND canon.paper_id <> sp.paper_id
  );

UPDATE subject_papers sp
INNER JOIN curriculum_subjects cs
    ON cs.curriculum_subject_id = sp.curriculum_subject_id
SET
    sp.paper_code = CONCAT('LEGACY-245-', sp.paper_id),
    sp.status = 'Inactive'
WHERE sp.subject_id = 1
  AND sp.academic_level_id = 2
  AND cs.curriculum_id = @uace_curriculum_id
  AND sp.paper_code = '245'
  AND EXISTS (
      SELECT 1
      FROM subject_papers canon
      WHERE canon.curriculum_subject_id = sp.curriculum_subject_id
        AND canon.paper_code = 'P425/2'
        AND canon.paper_id <> sp.paper_id
  );

-- Current aligned A-Level results must not inherit historical 6-point values.
UPDATE student_subject_results ssr
INNER JOIN examination_subjects es
    ON es.examination_subject_id = ssr.examination_subject_id
INNER JOIN examinations e
    ON e.examination_id = es.examination_id
INNER JOIN classes c
    ON c.class_id = e.class_id
INNER JOIN curricula cur
    ON cur.curriculum_code = 'UACE_ALIGNED'
SET ssr.grade_point = NULL
WHERE c.academic_level_id = cur.academic_level_id
  AND ssr.status = 'Calculated';

-- Any result that is not complete stays Pending.
-- For aligned A-Level, the curriculum master paper count is the required count:
-- a one-paper assignment for a two-paper subject cannot produce a final subject grade.
UPDATE student_subject_results ssr
INNER JOIN enrollments en
    ON en.enrollment_id = ssr.enrollment_id
INNER JOIN examination_subjects es
    ON es.examination_subject_id = ssr.examination_subject_id
INNER JOIN examinations ex
    ON ex.examination_id = es.examination_id
INNER JOIN classes cl
    ON cl.class_id = ex.class_id
INNER JOIN curricula cur
    ON cur.curriculum_code = 'UACE_ALIGNED'
    AND cur.academic_level_id = cl.academic_level_id
    AND cur.status = 'Active'
SET
    ssr.status = 'Pending',
    ssr.grade = NULL,
    ssr.grade_point = NULL,
    ssr.grade_weight = NULL,
    ssr.remarks = CONCAT(
        'Pending: ',
        (
            SELECT COUNT(*)
            FROM examination_papers ep
            INNER JOIN subject_papers sp
                ON sp.paper_id = ep.paper_id
            INNER JOIN curriculum_subjects cs
                ON cs.curriculum_subject_id = sp.curriculum_subject_id
            WHERE ep.examination_subject_id = ssr.examination_subject_id
              AND ep.status = 'Active'
              AND sp.status = 'Active'
              AND cs.curriculum_id = cur.curriculum_id
              AND cs.subject_id = es.subject_id
        ),
        ' required paper(s); ',
        (
            SELECT COUNT(*)
            FROM marks m
            INNER JOIN examination_papers ep2
                ON ep2.examination_paper_id = m.examination_paper_id
            WHERE ep2.examination_subject_id = ssr.examination_subject_id
              AND ep2.status = 'Active'
              AND m.student_id = en.student_id
        ),
        ' paper(s) currently marked.'
    )
WHERE
    (
        SELECT COUNT(*)
        FROM subject_papers spm
        INNER JOIN curriculum_subjects csm
            ON csm.curriculum_subject_id = spm.curriculum_subject_id
        WHERE csm.curriculum_id = cur.curriculum_id
          AND csm.subject_id = es.subject_id
          AND csm.status = 'Active'
          AND spm.status = 'Active'
    )
    <>
    (
        SELECT COUNT(*)
        FROM examination_papers epm
        INNER JOIN subject_papers spm2
            ON spm2.paper_id = epm.paper_id
        INNER JOIN curriculum_subjects csm2
            ON csm2.curriculum_subject_id = spm2.curriculum_subject_id
        WHERE epm.examination_subject_id = ssr.examination_subject_id
          AND epm.status = 'Active'
          AND spm2.status = 'Active'
          AND csm2.curriculum_id = cur.curriculum_id
          AND csm2.subject_id = es.subject_id
    )
    OR
    (
        SELECT COUNT(*)
        FROM marks mm
        INNER JOIN examination_papers ep3
            ON ep3.examination_paper_id = mm.examination_paper_id
        WHERE ep3.examination_subject_id = ssr.examination_subject_id
          AND ep3.status = 'Active'
          AND mm.student_id = en.student_id
    )
    <
    (
        SELECT COUNT(*)
        FROM examination_papers ep4
        WHERE ep4.examination_subject_id = ssr.examination_subject_id
          AND ep4.status = 'Active'
    );

-- End of migration.
