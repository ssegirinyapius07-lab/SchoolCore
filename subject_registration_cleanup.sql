-- SchoolCore: allow general subjects to exist without a UNEB code.
-- UNEB subject codes are maintained separately by curriculum.
-- Existing codes are preserved.

ALTER TABLE subjects
    MODIFY COLUMN subject_code VARCHAR(100) NULL;