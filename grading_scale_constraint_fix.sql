-- Fix the grading-scale uniqueness rule.
-- A single scale (for example O-Level Standard) must contain
-- multiple grades such as A, B, C, D and E.

ALTER TABLE grading_scales
    DROP INDEX uq_grading_scale_level_name;

ALTER TABLE grading_scales
    ADD UNIQUE KEY uq_grading_scale_level_name_grade
    (academic_level_id, scale_name, grade);
