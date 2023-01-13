#ifndef FPS
#define FPS 24 // default fps
#endif

#define MILLIS_TO_FRAMES(ms) ((ms * FPS) / 1000)

#define PERIODIC_EVENTS_CNT 16

uint16_t periodic_events = 0xFFFF;


uint16_t register_periodic_event(){
  static uint16_t cnt = 0, tmp = cnt;
  cnt = (++cnt) % PERIODIC_EVENTS_CNT;
  return tmp;
}


#define PERIODIC_EVENT(event, event_num, arduboy, frames_period) { \
    if (arduboy.everyXFrames(frames_period)) { \
      periodic_events ^= (1 << event_num); \
    } \
    if ((periodic_events & (1 << event_num)) >> event_num) { \
      event; \
    } \
  }