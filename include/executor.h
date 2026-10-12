#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"
#include "jobs.h"

int execute_command(command_t *cmd);

int execute_pipeline(pipeline_t *pipeline,
                     const char *command_line);

void setup_background_handler(void);

void setup_job_control(void);

int foreground_job(job_t *job);

int background_job(job_t *job);

#endif
