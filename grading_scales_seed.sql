-- SchoolCore starter grading configuration.
-- These values are configurable school rules, not hard-coded C++ logic.
-- Review/adjust the ranges before using them for live results.

-- O-Level: A-E achievement descriptors aligned with the UCE NLSC descriptors.
INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT
    al.academic_level_id,
    'O-Level Standard',
    80.00,
    100.00,
    'A',
    NULL,
    'Exceptional',
    'Active'
FROM academic_levels al
WHERE al.level_code = 'O_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'O-Level Standard'
        AND gs.grade = 'A'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'O-Level Standard', 70.00, 79.99, 'B', NULL, 'Outstanding', 'Active'
FROM academic_levels al
WHERE al.level_code = 'O_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'O-Level Standard'
        AND gs.grade = 'B'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'O-Level Standard', 60.00, 69.99, 'C', NULL, 'Satisfactory', 'Active'
FROM academic_levels al
WHERE al.level_code = 'O_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'O-Level Standard'
        AND gs.grade = 'C'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'O-Level Standard', 50.00, 59.99, 'D', NULL, 'Basic', 'Active'
FROM academic_levels al
WHERE al.level_code = 'O_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'O-Level Standard'
        AND gs.grade = 'D'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'O-Level Standard', 0.00, 49.99, 'E', NULL, 'Elementary', 'Active'
FROM academic_levels al
WHERE al.level_code = 'O_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'O-Level Standard'
        AND gs.grade = 'E'
  );


-- A-Level: percentage-to-grade conversion used by this school's
-- configurable internal grading policy.
-- Principal grade points follow the standard UACE A=6 ... F=0 scale.
INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'A-Level Standard', 80.00, 100.00, 'A', 6.00, 'Excellent', 'Active'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'A-Level Standard'
        AND gs.grade = 'A'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'A-Level Standard', 70.00, 79.99, 'B', 5.00, 'Very Good', 'Active'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'A-Level Standard'
        AND gs.grade = 'B'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'A-Level Standard', 60.00, 69.99, 'C', 4.00, 'Good', 'Active'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'A-Level Standard'
        AND gs.grade = 'C'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'A-Level Standard', 50.00, 59.99, 'D', 3.00, 'Satisfactory', 'Active'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'A-Level Standard'
        AND gs.grade = 'D'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'A-Level Standard', 40.00, 49.99, 'E', 2.00, 'Pass', 'Active'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'A-Level Standard'
        AND gs.grade = 'E'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'A-Level Standard', 30.00, 39.99, 'O', 1.00, 'Ordinary', 'Active'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'A-Level Standard'
        AND gs.grade = 'O'
  );

INSERT INTO grading_scales
    (academic_level_id, scale_name, min_score, max_score, grade, grade_point, remarks, status)
SELECT al.academic_level_id, 'A-Level Standard', 0.00, 29.99, 'F', 0.00, 'Fail', 'Active'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1 FROM grading_scales gs
      WHERE gs.academic_level_id = al.academic_level_id
        AND gs.scale_name = 'A-Level Standard'
        AND gs.grade = 'F'
  );
