#pragma once
#include <cstdlib>

struct RunServerArgs {
    int port;
    int numberOfSectors;
    int treasureIndex;
};

bool ParseArgs(RunServerArgs& args, int argc, const char* argv[]) {
    if (argc != 4) {
        return false;
    }

    args.port = atoi(argv[1]);
    args.numberOfSectors = atoi(argv[2]);
    args.treasureIndex = atoi(argv[3]);

    if (args.port * args.numberOfSectors * args.treasureIndex == 0) {
        return false;
    }

    return args.port > 0 && args.numberOfSectors > 0 && --args.treasureIndex >= 0;
}

struct RunServerWithMonitorArgs {
    int port;
    int monitorPort;
    int numberOfSectors;
    int treasureIndex;
};

bool ParseArgs(RunServerWithMonitorArgs& args, int argc, const char* argv[]) {
    if (argc != 5) {
        return false;
    }

    args.port = atoi(argv[1]);
    args.monitorPort = atoi(argv[2]);
    args.numberOfSectors = atoi(argv[3]);
    args.treasureIndex = atoi(argv[4]);

    if (args.port * args.monitorPort * args.numberOfSectors * args.treasureIndex == 0) {
        return false;
    }

    return args.port > 0 && args.monitorPort > 0 && args.numberOfSectors > 0 && --args.treasureIndex >= 0;
}

struct RunMonitorArgs {
    const char* server;
    int port;
    int numberOfSectors;
};

bool ParseArgs(RunMonitorArgs &args, int argc, const char **argv) {
    if (argc != 4) {
        return false;
    }

    args.server = argv[1];
    args.port = atoi(argv[2]);
    args.numberOfSectors = atoi(argv[3]);

    return args.port > 0 && args.numberOfSectors > 0;
}


struct RunClientArgs {
    const char* server;
    int port;
};

bool ParseArgs(RunClientArgs &args, int argc, const char **argv) {
    if (argc != 3) {
        return false;
    }

    args.server = argv[1];
    args.port = atoi(argv[2]);

    return args.port > 0;
}

struct ThreadArgs {
    int socket;
    int numberOfSectors;
};

struct SendData {
    int selectedSector;
    bool hasTreasure;
};

struct RecievedData {
    bool treasureFound;
    bool isAlive = true;
};
