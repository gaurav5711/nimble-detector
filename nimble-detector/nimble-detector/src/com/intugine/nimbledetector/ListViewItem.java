/* UNIMPORTANT
 * UNIMPORTANT
 * UNIMPORTANT
 * UNIMPORTANT
*/




package com.intugine.nimbledetector;

public class ListViewItem {

    private String m_szDeviceName;
    private String m_szDevicePath;
    private boolean m_bConnected;
    private int m_nDeviceID, m_nConType;

    public ListViewItem( String deviceName, String DevicePath, boolean deviceStatus, int deviceID ) {
        m_szDeviceName = deviceName;
        m_szDevicePath = DevicePath;
        m_bConnected = deviceStatus;
        m_nDeviceID = deviceID;
    }


    public String getDeviceName() { return m_szDeviceName; }
    public void setDeviceName(String deviceName) { m_szDeviceName = deviceName;}

    public String getDevicePath() {return m_szDevicePath;}
    public void setDevicePath(String DevicePath) {m_szDevicePath = DevicePath;}


    public boolean getDeviceStatus() { return m_bConnected; }
    public void setDeviceStatus(boolean deviceStatus) { m_bConnected = deviceStatus;}

    public int getDeviceID() { return m_nDeviceID; }
    public void setDeviceID(int deviceID) { m_nDeviceID = deviceID;}
}

