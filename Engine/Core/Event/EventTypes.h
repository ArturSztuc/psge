#pragma once
/** 
 * @enum EventType
 * @brief Lists types of events 
 * This will grow as we expand the engine. Should probably be able to have
 * custom event types defined in the client? Not sure how to implement it for
 * now though.
 */
enum EventType 
{  
  EVENT_TYPE_WINDOW,
  EVENT_TYPE_KEYBOARD_INPUT,
  EVENT_TYPE_GAME,
  EVENT_TYPE_COLLISION
};
