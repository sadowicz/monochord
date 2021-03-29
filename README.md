# Monochord

---
Low-level C programming project with sockets and IPC using Linux signals. It consists of three programs.
## monochrd
- **Program name:** *monochord*
- **Parameters:** *<short_int>* - port number (positional parameter)
- **Example Usage:** `./monochord 8888`
## recorder
- **Program name:** *recorder*
- **Parameters:**
    - *-d <int>* - real-time signal number (signal with data from monochord)
    - *-c <int>* - real-time signal number (signal with coded control commands)
    - *-b <string>* - binary file path (optional - default: none)
    - *-t <string>* - text file path (optional - default: stdout)
- **Example Usage:** `./recorder -d 47 -c 48 binaryFile textFile`
## info_ rejestrator
- **Program name:** *info_rejestrator*
- **Parameters:**
    - *-c <int>* - real-time signal number
    - *<int>* - PID (positional parameter)
- **Example Usage:** `./info_ rejestrator -c 48 8790`
