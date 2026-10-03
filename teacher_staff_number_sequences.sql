CREATE TABLE IF NOT EXISTS teacher_staff_number_sequences (
    staff_year INT NOT NULL,
    last_sequence INT NOT NULL DEFAULT 0,
    PRIMARY KEY (staff_year)
);