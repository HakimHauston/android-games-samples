#!/bin/bash

read -r -d '' CONFIG << EOM
buffers: {
    size_kb: 634888
    fill_policy: RING_BUFFER
}
buffers: {
    size_kb: 2048
    fill_policy: RING_BUFFER
}
data_sources: {
    config {
        name: "android.gpu.memory"
    }
}
data_sources: {
    config {
        name: "android.surfaceflinger.frametimeline"
    }
}
data_sources: {
    config {
        name: "linux.process_stats"
        target_buffer: 1
        process_stats_config {
            scan_all_processes_on_start: true
        }
    }
}

data_sources: {
    config {
        name: "linux.sys_stats"
        sys_stats_config {
            stat_period_ms: 1000
            stat_counters: STAT_CPU_TIMES
            stat_counters: STAT_FORK_COUNT
            cpufreq_period_ms: 1000
        }

    }
}
data_sources: {
    config {
        name: "android.power"
        android_power_config {
            battery_poll_ms: 1000
            battery_counters: BATTERY_COUNTER_CAPACITY_PERCENT
            battery_counters: BATTERY_COUNTER_CHARGE
            battery_counters: BATTERY_COUNTER_CURRENT
            collect_power_rails: true
        }
    }
}
data_sources: {
    config {
        name: "linux.ftrace"
        ftrace_config {
            ftrace_events: "thermal/*"
            ftrace_events: "sched/sched_switch"
            ftrace_events: "power/suspend_resume"
            ftrace_events: "sched/sched_wakeup"
            ftrace_events: "sched/sched_wakeup_new"
            ftrace_events: "sched/sched_waking"
            ftrace_events: "power/cpu_frequency"
            ftrace_events: "power/gpu_frequency"
            ftrace_events: "power/cpu_idle"
            ftrace_events: "gpu_mem/gpu_mem_total"
            ftrace_events: "power/clock_enable"
            ftrace_events: "power/clock_disable"
            ftrace_events: "power/clock_set_rate"
            # ftrace_events: "raw_syscalls/sys_enter"
            # ftrace_events: "raw_syscalls/sys_exit"
            ftrace_events: "sched/sched_process_exit"
            ftrace_events: "sched/sched_process_free"
            ftrace_events: "task/task_newtask"
            ftrace_events: "task/task_rename"
            ftrace_events: "ftrace/print"
            atrace_categories: "gfx"
            atrace_categories: "input"
            atrace_categories: "view"
            atrace_categories: "wm"
            atrace_categories: "am"
            atrace_categories: "camera"
            atrace_categories: "power"
            atrace_categories: "thermal"
            atrace_categories: "pm"
            atrace_categories: "dalvik"
            atrace_categories: "irq"
            atrace_categories: "aidl"
            atrace_categories: "ss"
            atrace_apps: "*"
            buffer_size_kb: 2048
            drain_period_ms: 250
        }
    }
}
data_sources: {
    config {
        name: "org.chromium.trace_event"
        chrome_config {
            trace_config: "{\"record_mode\":\"record-until-full\",\"included_categories\":[\"toplevel\",\"cc\",\"gpu\",\"viz\",\"ui\",\"views\"],\"memory_dump_config\":{}}"
        }
    }
}
data_sources: {
    config {
        name: "track_event"
        target_buffer: 0
        statsd_tracing_config {
          push_atom_id: ATOM_THERMAL_THROTTLING_SEVERITY_STATE_CHANGED
        }
    }
}
data_sources {
  config {
    name: "gpu.counters"
    gpu_counter_config {
      counter_period_ns: 10000000
      counter_ids: 65572
      counter_ids: 65571
      counter_ids: 65570
      counter_ids: 65569
      counter_ids: 65567
      counter_ids: 65566
      counter_ids: 65565
      counter_ids: 65564
      counter_ids: 65563
      counter_ids: 65562
      counter_ids: 65561
      counter_ids: 65560
      counter_ids: 65559
      counter_ids: 65558
      counter_ids: 65557
      counter_ids: 65556
      counter_ids: 65555
      counter_ids: 65554
      counter_ids: 65553
      counter_ids: 65552
      counter_ids: 65551
      counter_ids: 65550
      counter_ids: 65549
      counter_ids: 65548
      counter_ids: 65547
      counter_ids: 65546
      counter_ids: 65545
      counter_ids: 65544
      counter_ids: 65543
      counter_ids: 65542
      counter_ids: 65541
      counter_ids: 65540
      counter_ids: 65539
      counter_ids: 65538
      counter_ids: 65537
      counter_ids: 254
      counter_ids: 253
      counter_ids: 255
      counter_ids: 252
      counter_ids: 251
      counter_ids: 250
      counter_ids: 249
      counter_ids: 248
      counter_ids: 247
      counter_ids: 246
      counter_ids: 244
      counter_ids: 243
      counter_ids: 242
      counter_ids: 241
      counter_ids: 240
      counter_ids: 239
      counter_ids: 238
      counter_ids: 237
      counter_ids: 236
      counter_ids: 233
      counter_ids: 232
      counter_ids: 230
      counter_ids: 229
      counter_ids: 234
      counter_ids: 235
      counter_ids: 231
      counter_ids: 200
      counter_ids: 199
      counter_ids: 198
      counter_ids: 197
      counter_ids: 196
      counter_ids: 245
      counter_ids: 127
      counter_ids: 126
      counter_ids: 125
      counter_ids: 124
      counter_ids: 123
      counter_ids: 122
      counter_ids: 121
      counter_ids: 120
      counter_ids: 119
      counter_ids: 118
      counter_ids: 117
      counter_ids: 116
      counter_ids: 115
      counter_ids: 114
      counter_ids: 113
      counter_ids: 112
      counter_ids: 111
      counter_ids: 110
      counter_ids: 109
      counter_ids: 108
      counter_ids: 107
      counter_ids: 106
      counter_ids: 65610
      counter_ids: 105
      counter_ids: 65609
      counter_ids: 104
      counter_ids: 65608
      counter_ids: 103
      counter_ids: 65607
      counter_ids: 102
      counter_ids: 65606
      counter_ids: 101
      counter_ids: 65605
      counter_ids: 100
      counter_ids: 65604
      counter_ids: 99
      counter_ids: 65603
      counter_ids: 98
      counter_ids: 65602
      counter_ids: 97
      counter_ids: 65601
      counter_ids: 96
      counter_ids: 65600
      counter_ids: 95
      counter_ids: 65599
      counter_ids: 94
      counter_ids: 65598
      counter_ids: 93
      counter_ids: 65597
      counter_ids: 92
      counter_ids: 65596
      counter_ids: 91
      counter_ids: 65595
      counter_ids: 90
      counter_ids: 65594
      counter_ids: 89
      counter_ids: 65593
      counter_ids: 88
      counter_ids: 65592
      counter_ids: 87
      counter_ids: 65591
      counter_ids: 86
      counter_ids: 65590
      counter_ids: 85
      counter_ids: 184
      counter_ids: 183
      counter_ids: 182
      counter_ids: 65589
      counter_ids: 84
      counter_ids: 181
      counter_ids: 65588
      counter_ids: 83
      counter_ids: 180
      counter_ids: 65587
      counter_ids: 82
      counter_ids: 179
      counter_ids: 65586
      counter_ids: 81
      counter_ids: 178
      counter_ids: 65585
      counter_ids: 80
      counter_ids: 177
      counter_ids: 65584
      counter_ids: 79
      counter_ids: 176
      counter_ids: 65583
      counter_ids: 78
      counter_ids: 175
      counter_ids: 65582
      counter_ids: 77
      counter_ids: 174
      counter_ids: 65581
      counter_ids: 76
      counter_ids: 173
      counter_ids: 65580
      counter_ids: 75
      counter_ids: 172
      counter_ids: 65579
      counter_ids: 74
      counter_ids: 171
      counter_ids: 65578
      counter_ids: 73
      counter_ids: 170
      counter_ids: 65577
      counter_ids: 72
      counter_ids: 169
      counter_ids: 65576
      counter_ids: 71
      counter_ids: 168
      counter_ids: 65575
      counter_ids: 70
      counter_ids: 167
      counter_ids: 65574
      counter_ids: 69
      counter_ids: 166
      counter_ids: 65573
      counter_ids: 68
      counter_ids: 165
      counter_ids: 164
      counter_ids: 163
      counter_ids: 162
      counter_ids: 161
      counter_ids: 159
      counter_ids: 158
      counter_ids: 157
      counter_ids: 156
      counter_ids: 155
      counter_ids: 154
      counter_ids: 153
      counter_ids: 152
      counter_ids: 144
      counter_ids: 141
      counter_ids: 140
      counter_ids: 139
      counter_ids: 138
      counter_ids: 137
      counter_ids: 136
      counter_ids: 135
      counter_ids: 134
      counter_ids: 133
      counter_ids: 132
      counter_ids: 228
      counter_ids: 65536
      counter_ids: 31
      counter_ids: 227
      counter_ids: 65535
      counter_ids: 30
      counter_ids: 226
      counter_ids: 29
      counter_ids: 225
      counter_ids: 28
      counter_ids: 224
      counter_ids: 27
      counter_ids: 223
      counter_ids: 26
      counter_ids: 222
      counter_ids: 25
      counter_ids: 221
      counter_ids: 24
      counter_ids: 220
      counter_ids: 23
      counter_ids: 219
      counter_ids: 22
      counter_ids: 218
      counter_ids: 21
      counter_ids: 217
      counter_ids: 20
      counter_ids: 216
      counter_ids: 19
      counter_ids: 215
      counter_ids: 18
      counter_ids: 214
      counter_ids: 17
      counter_ids: 160
      counter_ids: 65568
      counter_ids: 63
      counter_ids: 213
      counter_ids: 16
      counter_ids: 212
      counter_ids: 15
      counter_ids: 211
      counter_ids: 14
      counter_ids: 210
      counter_ids: 13
      counter_ids: 209
      counter_ids: 12
      counter_ids: 208
      counter_ids: 11
      counter_ids: 151
      counter_ids: 207
      counter_ids: 10
      counter_ids: 150
      counter_ids: 206
      counter_ids: 9
      counter_ids: 149
      counter_ids: 205
      counter_ids: 8
      counter_ids: 148
      counter_ids: 204
      counter_ids: 7
      counter_ids: 147
      counter_ids: 203
      counter_ids: 6
      counter_ids: 146
      counter_ids: 202
      counter_ids: 5
      counter_ids: 145
      counter_ids: 201
      counter_ids: 4
    }
  }
} 

EOM

echo "$CONFIG" > .temp_perfetto_config.txt
adb root
adb push .temp_perfetto_config.txt /data/local/tmp/trace_config.txt
rm -f .temp_perfetto_config.txt
adb shell 'cat /data/local/tmp/trace_config.txt | perfetto --txt -c - -o /data/misc/perfetto-traces/trace -d' > /dev/null
echo "Trace Started. Press any key to stop tracing..."
read 
echo "Done. Pulling trace"
adb shell killall perfetto
sleep 7 
adb pull /data/misc/perfetto-traces/trace ~/Downloads/trace
