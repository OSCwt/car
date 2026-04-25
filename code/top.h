/*
 * top.h
 *
 *  创建时间: 2026年4月5日
 *      作者: 22563
 */

#ifndef CODE_TOP_H_
#define CODE_TOP_H_
//显示屏
#define LINE_HEIGHT 20
#define START_Y     0
#define LABEL_ROWS 15

extern uint16 label_ids[LABEL_ROWS];

void isp_init(void);
void task_init(void);
void control(void);


#endif /* CODE_TOP_H_ */
