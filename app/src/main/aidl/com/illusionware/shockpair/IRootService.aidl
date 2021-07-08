// IRootService.aidl
package com.illusionware.shockpair;

// Declare any non-default types here with import statements

interface IRootService {
    int getPid();
    int getUid();
    int connectToDS3();
}