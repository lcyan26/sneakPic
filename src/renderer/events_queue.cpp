#include "renderer/events_queue.h"

#include <QElapsedTimer>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>

#include "common/memory_deallocation.h"

#include "renderer/abstract_renderer_event.h"


events_queue::events_queue ()
{
  m_last_queue_id = 0;
  m_calculated_id = 0;
  m_mutex = new QMutex;
  m_wait_cond = new QWaitCondition;
}

events_queue::~events_queue ()
{
  abstract_renderer_event *ev = 0;
  while (!m_queue.isEmpty ())
    {
      ev = m_queue.dequeue ();
      FREE (ev);
    }
}


int events_queue::add_event (abstract_renderer_event *ev)
{
  QMutexLocker ml (m_mutex);
  m_queue.enqueue (ev);
  m_last_queue_id++;
  ev->set_queue_id (m_last_queue_id);
  m_wait_cond->wakeOne ();
  return m_last_queue_id;
}

abstract_renderer_event *events_queue::pull_event ()
{
  if (!m_queue.isEmpty ())
    return m_queue.dequeue ();
  else
    return 0;
}

void events_queue::timed_wait (unsigned long time)
{
  m_wait_cond->wait (m_mutex, time);
}

void events_queue::wait_for_id (int id, unsigned long time_ms)
{
  QMutexLocker ml (m_mutex);
  QElapsedTimer timer;
  timer.start();
  while (id > m_calculated_id)
    {
      if (time_ms <= timer.elapsed ())
        return;

      m_wait_cond->wait (m_mutex, time_ms - timer.elapsed ());
    }
}

void events_queue::set_calculated_id (int id)
{
  QMutexLocker ml (m_mutex);
  m_calculated_id = id;
  m_wait_cond->wakeOne ();
}

void events_queue::lock ()
{
  m_mutex->lock ();
}

void events_queue::unlock ()
{
  m_mutex->unlock ();
}

int events_queue::empty () const
{
  return m_queue.isEmpty ();
}


