-- SchoolCore: classify O-Level curriculum subjects as Mandatory or Optional.
-- Run once in the schoolcore_db database.
-- The eight compulsory O-Level subjects are:
-- Mathematics, Physics, Chemistry, Biology, English Language,
-- Geography, History & Political Education, Religious Education.
-- All other O-Level curriculum subjects remain Optional.

ALTER TABLE curriculum_subjects
    ADD COLUMN IF NOT EXISTS requirement_type VARCHAR(20) NOT NULL DEFAULT 'Optional'
    AFTER subject_group;

UPDATE curriculum_subjects cs
INNER JOIN curricula c
    ON c.curriculum_id = cs.curriculum_id
INNER JOIN academic_levels al
    ON al.academic_level_id = c.academic_level_id
INNER JOIN subjects s
    ON s.subject_id = cs.subject_id
SET cs.requirement_type = 'Mandatory'
WHERE al.level_code = 'O_LEVEL'
  AND s.subject_name IN (
      'Mathematics',
      'Physics',
      'Chemistry',
      'Biology',
      'English Language',
      'Geography',
      'History & Political Education',
      'Religious Education'
  );

UPDATE curriculum_subjects cs
INNER JOIN curricula c
    ON c.curriculum_id = cs.curriculum_id
INNER JOIN academic_levels al
    ON al.academic_level_id = c.academic_level_id
SET cs.requirement_type = 'Optional'
WHERE al.level_code = 'O_LEVEL'
  AND cs.requirement_type NOT IN ('Mandatory');
