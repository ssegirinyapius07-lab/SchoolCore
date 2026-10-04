-- Fix the grading-scale uniqueness rule.
-- A single scale (for example O-Level Standard) must contain
-- multiple grades such as A, B, C, D and E.
--
-- The old unique index is also being used by the academic-level
-- foreign key, so remove and restore that foreign key safely.

ALTER TABLE grading_scales
    DROP FOREIGN KEY fk_grading_scales_level;

ALTER TABLE grading_scales
    DROP INDEX uq_grading_scale_level_name;

ALTER TABLE grading_scales
    ADD UNIQUE KEY uq_grading_scale_level_name_grade
    (academic_level_id, scale_name, grade);

ALTER TABLE grading_scales
    ADD CONSTRAINT fk_grading_scales_level
    FOREIGN KEY (academic_level_id)
    REFERENCES academic_levels (academic_level_id)
    ON UPDATE CASCADE
    ON DELETE RESTRICT;
