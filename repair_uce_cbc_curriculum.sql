-- SchoolCore: repair and seed the UCE CBC O-Level curriculum catalogue.
-- Run this once in schoolcore_db after the existing curriculum/UNEB migrations.
--
-- Purpose:
-- 1. Link the active 2026 academic year to the UCE CBC curriculum.
-- 2. Remove the incorrect O-Level Biology curriculum row that was using
--    UNEB code 245.
-- 3. Add the verified UCE/NLSC core subjects to the UCE CBC curriculum.
-- 4. Add several existing optional subjects using verified UNEB subject codes.
-- 5. Seed official NLSC paper codes currently documented by UNEB.
--
-- UNEB references used:
-- - 2025 secondary registration circular: 7 compulsory UCE subjects.
-- - 2024 UCE New Lower Secondary Curriculum timetable: paper codes.
--
-- No unofficial paper codes are generated here.

START TRANSACTION;

-- ------------------------------------------------------------
-- 1. The 2026 academic year uses the UCE CBC curriculum.
-- ------------------------------------------------------------
UPDATE academic_years ay
INNER JOIN curricula c
    ON c.curriculum_code = 'UCE_CBC'
   AND c.academic_level_id = 1
   AND c.status = 'Active'
SET ay.curriculum_id = c.curriculum_id
WHERE ay.year_name = '2026'
  AND (ay.curriculum_id IS NULL OR ay.curriculum_id <> c.curriculum_id);

-- ------------------------------------------------------------
-- 2. Remove the known bad O-Level curriculum mapping:
--    Biology was incorrectly stored as UNEB code 245.
-- ------------------------------------------------------------
DELETE cs
FROM curriculum_subjects cs
INNER JOIN curricula c
    ON c.curriculum_id = cs.curriculum_id
WHERE c.curriculum_code = 'UCE_CBC'
  AND cs.subject_id = (
      SELECT s.subject_id
      FROM subjects s
      WHERE s.subject_name = 'Biology'
      LIMIT 1
  )
  AND cs.uneb_subject_code = '245';

-- ------------------------------------------------------------
-- 3. UCE CBC compulsory subjects.
--    UNEB codes verified from official UCE registration/timetable
--    material: 112, 456, 535, 545, 553, 273, 241.
-- ------------------------------------------------------------
INSERT INTO curriculum_subjects (
    curriculum_id,
    subject_id,
    uneb_subject_code,
    subject_group,
    requirement_type,
    status
)
SELECT
    c.curriculum_id,
    s.subject_id,
    x.uneb_subject_code,
    x.subject_group,
    'Mandatory',
    'Active'
FROM (
    SELECT 'Mathematics' AS subject_name, '456' AS uneb_subject_code, 'Mathematics' AS subject_group
    UNION ALL
    SELECT 'Physics', '535', 'Sciences'
    UNION ALL
    SELECT 'Chemistry', '545', 'Sciences'
    UNION ALL
    SELECT 'Biology', '553', 'Sciences'
    UNION ALL
    SELECT 'English Language', '112', 'Languages'
    UNION ALL
    SELECT 'Geography', '273', 'Humanities'
    UNION ALL
    SELECT 'History & Political Education', '241', 'Humanities'
) x
INNER JOIN subjects s
    ON s.subject_name = x.subject_name
INNER JOIN curricula c
    ON c.curriculum_code = 'UCE_CBC'
   AND c.academic_level_id = 1
   AND c.status = 'Active'
ON DUPLICATE KEY UPDATE
    uneb_subject_code = VALUES(uneb_subject_code),
    subject_group = VALUES(subject_group),
    requirement_type = VALUES(requirement_type),
    status = VALUES(status);

-- ------------------------------------------------------------
-- 4. Existing optional subjects whose UNEB codes are verified
--    in the NLSC timetable.
-- ------------------------------------------------------------
INSERT INTO curriculum_subjects (
    curriculum_id,
    subject_id,
    uneb_subject_code,
    subject_group,
    requirement_type,
    status
)
SELECT
    c.curriculum_id,
    s.subject_id,
    x.uneb_subject_code,
    x.subject_group,
    'Optional',
    'Active'
FROM (
    SELECT 'Literature in English' AS subject_name, '208' AS uneb_subject_code, 'Languages' AS subject_group
    UNION ALL
    SELECT 'Kiswahili', '336', 'Languages'
    UNION ALL
    SELECT 'Physical Education', '555', 'Other'
    UNION ALL
    SELECT 'Entrepreneurship', '845', 'Other'
) x
INNER JOIN subjects s
    ON s.subject_name = x.subject_name
INNER JOIN curricula c
    ON c.curriculum_code = 'UCE_CBC'
   AND c.academic_level_id = 1
   AND c.status = 'Active'
ON DUPLICATE KEY UPDATE
    uneb_subject_code = VALUES(uneb_subject_code),
    subject_group = VALUES(subject_group),
    requirement_type = VALUES(requirement_type),
    status = VALUES(status);

-- Religious Education remains optional in the database until the
-- generic subject is split into Christian Religious Education (223)
-- and Islamic Religious Education (225). We do not invent a combined
-- UNEB subject code such as 223/225.

UPDATE curriculum_subjects cs
INNER JOIN curricula c
    ON c.curriculum_id = cs.curriculum_id
INNER JOIN subjects s
    ON s.subject_id = cs.subject_id
SET cs.requirement_type = 'Optional'
WHERE c.curriculum_code = 'UCE_CBC'
  AND s.subject_name = 'Religious Education';

-- ------------------------------------------------------------
-- 5. Remove the known incorrect A-Level paper row:
--    paper 245 was stored under A-Level Mathematics but named Biology.
-- ------------------------------------------------------------
DELETE FROM subject_papers
WHERE paper_id = 2
  AND curriculum_subject_id = 1
  AND subject_id = 1
  AND academic_level_id = 2
  AND paper_code = '245'
  AND paper_name = 'Biology';

-- ------------------------------------------------------------
-- 6. Verified UCE/NLSC paper master data.
--    These paper codes are taken from UNEB's official NLSC
--    examination timetable.
-- ------------------------------------------------------------
INSERT INTO subject_papers (
    curriculum_subject_id,
    subject_id,
    academic_level_id,
    paper_code,
    paper_number,
    paper_name,
    paper_type,
    status
)
SELECT
    cs.curriculum_subject_id,
    s.subject_id,
    1,
    x.paper_code,
    x.paper_number,
    x.paper_name,
    x.paper_type,
    'Active'
FROM (
    SELECT 'Mathematics' AS subject_name, '456/1' AS paper_code, '1' AS paper_number, 'Mathematics' AS paper_name, 'Theory' AS paper_type
    UNION ALL
    SELECT 'Physics', '535/1', '1', 'Physics', 'Theory'
    UNION ALL
    SELECT 'Physics', '535/2', '2', 'Physics Practical', 'Practical'
    UNION ALL
    SELECT 'Physics', '535/3', '3', 'Physics Practical', 'Practical'
    UNION ALL
    SELECT 'Chemistry', '545/1', '1', 'Chemistry', 'Theory'
    UNION ALL
    SELECT 'Chemistry', '545/2', '2', 'Chemistry Practical', 'Practical'
    UNION ALL
    SELECT 'Chemistry', '545/3', '3', 'Chemistry Practical', 'Practical'
    UNION ALL
    SELECT 'Biology', '553/1', '1', 'Biology', 'Theory'
    UNION ALL
    SELECT 'Biology', '553/2', '2', 'Biology Practical', 'Practical'
    UNION ALL
    SELECT 'Biology', '553/3', '3', 'Biology Practical', 'Practical'
    UNION ALL
    SELECT 'Geography', '273/1', '1', 'Geography', 'Theory'
    UNION ALL
    SELECT 'English Language', '112/1', '1', 'English Language', 'Theory'
    UNION ALL
    SELECT 'History & Political Education', '241/1', '1', 'History & Political Education', 'Theory'
    UNION ALL
    SELECT 'Literature in English', '208/1', '1', 'Literature in English', 'Theory'
    UNION ALL
    SELECT 'Kiswahili', '336/1', '1', 'Lugha na Fasihi ya Kiswahili', 'Theory'
    UNION ALL
    SELECT 'Kiswahili', '336/2', '2', 'Lugha na Fasihi ya Kiswahili', 'Theory'
    UNION ALL
    SELECT 'Physical Education', '555/1', '1', 'Physical Education', 'Theory'
    UNION ALL
    SELECT 'Entrepreneurship', '845/1', '1', 'Entrepreneurship', 'Theory'
) x
INNER JOIN subjects s
    ON s.subject_name = x.subject_name
INNER JOIN curricula c
    ON c.curriculum_code = 'UCE_CBC'
   AND c.academic_level_id = 1
   AND c.status = 'Active'
INNER JOIN curriculum_subjects cs
    ON cs.curriculum_id = c.curriculum_id
   AND cs.subject_id = s.subject_id
   AND cs.status = 'Active'
WHERE NOT EXISTS (
    SELECT 1
    FROM subject_papers existing
    WHERE existing.curriculum_subject_id = cs.curriculum_subject_id
      AND existing.paper_code = x.paper_code
);

COMMIT;

-- ------------------------------------------------------------
-- Verification
-- ------------------------------------------------------------
SELECT
    c.curriculum_code,
    s.subject_name,
    cs.uneb_subject_code,
    cs.requirement_type,
    cs.status,
    sp.paper_code,
    sp.paper_name,
    sp.paper_type,
    sp.status AS paper_status
FROM curriculum_subjects cs
INNER JOIN curricula c
    ON c.curriculum_id = cs.curriculum_id
INNER JOIN subjects s
    ON s.subject_id = cs.subject_id
LEFT JOIN subject_papers sp
    ON sp.curriculum_subject_id = cs.curriculum_subject_id
WHERE c.curriculum_code = 'UCE_CBC'
ORDER BY
    cs.requirement_type DESC,
    s.subject_name,
    sp.paper_code;
