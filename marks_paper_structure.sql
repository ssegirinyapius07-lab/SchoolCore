-- SchoolCore: link marks to the actual examination paper.
-- Run once in schoolcore_db.
--
-- The examination already has examination_papers records such as
-- Paper 1 / Paper 2 with official paper_code, paper_name, max_score
-- and pass_mark. Marks must be stored against that paper so that
-- the same student can have separate marks for multiple papers
-- belonging to the same subject.
--
-- The examination_subject foreign key currently depends on an index
-- named examination_subject_id, so that foreign key must be removed
-- before replacing the old subject-level unique indexes.

ALTER TABLE marks
    ADD COLUMN IF NOT EXISTS examination_paper_id INT(10) UNSIGNED NULL
    AFTER examination_subject_id;

-- Remove the subject-level foreign key temporarily so its supporting
-- index can be replaced.
ALTER TABLE marks
    DROP FOREIGN KEY fk_marks_exam_subject;

-- The previous subject-level uniqueness prevented Paper 1 and Paper 2
-- from being stored separately for the same student.
ALTER TABLE marks
    DROP INDEX examination_subject_id,
    DROP INDEX uq_marks_examination_subject_student;

-- Restore a normal index for the examination_subject foreign key.
ALTER TABLE marks
    ADD KEY idx_marks_examination_subject
    (examination_subject_id);

ALTER TABLE marks
    ADD CONSTRAINT fk_marks_exam_subject
    FOREIGN KEY (examination_subject_id)
    REFERENCES examination_subjects (examination_subject_id)
    ON DELETE CASCADE
    ON UPDATE CASCADE;

-- Each mark is now unique by the actual examination paper and student.
ALTER TABLE marks
    ADD KEY idx_marks_examination_paper_student
    (examination_paper_id, student_id);

ALTER TABLE marks
    ADD CONSTRAINT fk_marks_examination_paper
    FOREIGN KEY (examination_paper_id)
    REFERENCES examination_papers (examination_paper_id)
    ON DELETE CASCADE
    ON UPDATE CASCADE;

ALTER TABLE marks
    ADD UNIQUE KEY uq_marks_examination_paper_student
    (examination_paper_id, student_id);
