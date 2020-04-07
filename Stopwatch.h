//
// Created by Stijn on 05/03/2020.
//

#ifndef BRAINFCKINTERPRETER_STOPWATCH_H
#define BRAINFCKINTERPRETER_STOPWATCH_H

// Copyright Ingo Proff 2017.
// https://github.com/CrikeeIP/Stopwatch
// Distributed under the MIT Software License (X11 license).
// (See accompanying file LICENSE)

#pragma once

#include <vector>
#include <chrono>
#include <string>

namespace Stopwatch
{
    class Stopwatch
    {
    public:
        enum TimeFormat { NANOSECONDS, MICROSECONDS, MILLISECONDS, SECONDS };

        Stopwatch() : _startTime(), _laps({})
        {
        }

        void Start()
        {
            _startTime = std::chrono::high_resolution_clock::now();
            _laps = { _startTime };
        }

        template<TimeFormat fmt = TimeFormat::MILLISECONDS>
        std::uint64_t Lap()
        {
            const auto t = std::chrono::high_resolution_clock::now();
            const auto last_r = _laps.back();
            _laps.push_back(t);
            return Ticks<fmt>(last_r, t);
        }

        template<TimeFormat fmt = TimeFormat::MILLISECONDS>
        std::uint64_t Elapsed()
        {
            const auto end_time = std::chrono::high_resolution_clock::now();
            return Ticks<fmt>(_startTime, end_time);
        }

        template<TimeFormat fmt_total = TimeFormat::MILLISECONDS, TimeFormat fmt_lap = fmt_total>
        std::pair<std::uint64_t, std::vector<std::uint64_t>> ElapsedLaps()
        {
            std::vector<std::uint64_t> lap_times;
            lap_times.reserve(_laps.size() - 1);

            for (std::size_t idx = 0; idx <= _laps.size() - 2; idx++)
            {
                const auto lap_end = _laps[idx + 1];
                const auto lap_start = _laps[idx];
                lap_times.push_back(Ticks<fmt_lap>(lap_start, lap_end));
            }

            return {
                    Ticks<fmt_total>(_startTime, _laps.back()),
                    lap_times
            };
        }

        std::string StringifyTimes(const std::vector<std::uint64_t> &times)
        {
            std::string result("{");
            for (const auto &t : times)
            {
                result += std::to_string(t) + ",";
            }
            result.back() = static_cast<char>('}');
            return result;
        }

    private:
        typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_pt;
        time_pt _startTime;
        std::vector<time_pt> _laps;

        template<TimeFormat fmt = TimeFormat::MILLISECONDS>
        static std::uint64_t Ticks(const time_pt &start_time, const time_pt &end_time)
        {
            const auto duration = end_time - start_time;
            const std::uint64_t ns_count = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

            switch (fmt)
            {
                case TimeFormat::NANOSECONDS:
                {
                    return ns_count;
                }
                case TimeFormat::MICROSECONDS:
                {
                    std::uint64_t up = ((ns_count / 100) % 10 >= 5) ? 1 : 0;
                    const auto mus_count = (ns_count / 1000) + up;
                    return mus_count;
                }
                case TimeFormat::MILLISECONDS:
                {
                    std::uint64_t up = ((ns_count / 100000) % 10 >= 5) ? 1 : 0;
                    const auto ms_count = (ns_count / 1000000) + up;
                    return ms_count;
                }
                case TimeFormat::SECONDS:
                {
                    std::uint64_t up = ((ns_count / 100000000) % 10 >= 5) ? 1 : 0;
                    const auto s_count = (ns_count / 1000000000) + up;
                    return s_count;
                }
            }
        }
    };


    constexpr Stopwatch::TimeFormat ns = Stopwatch::TimeFormat::NANOSECONDS;
    constexpr Stopwatch::TimeFormat mus = Stopwatch::TimeFormat::MICROSECONDS;
    constexpr Stopwatch::TimeFormat ms = Stopwatch::TimeFormat::MILLISECONDS;
    constexpr Stopwatch::TimeFormat s = Stopwatch::TimeFormat::SECONDS;

    constexpr Stopwatch::TimeFormat nanoseconds = Stopwatch::TimeFormat::NANOSECONDS;
    constexpr Stopwatch::TimeFormat microseconds = Stopwatch::TimeFormat::MICROSECONDS;
    constexpr Stopwatch::TimeFormat milliseconds = Stopwatch::TimeFormat::MILLISECONDS;
    constexpr Stopwatch::TimeFormat seconds = Stopwatch::TimeFormat::SECONDS;

} //namespace stopwatch
#endif //BRAINFCKINTERPRETER_STOPWATCH_H
