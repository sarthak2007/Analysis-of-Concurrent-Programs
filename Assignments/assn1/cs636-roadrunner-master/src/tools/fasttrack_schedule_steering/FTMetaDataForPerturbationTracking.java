package tools.fasttrack_schedule_steering;

public class FTMetaDataForPerturbationTracking {

  private boolean isListening;
  private boolean isBAccessed;
  private int freq;
  private boolean isABRaceDetected;
  private boolean isBARaceDetected;

  public String sourceLocationForB;

  public FTMetaDataForPerturbationTracking(String b) {
    sourceLocationForB = b;
    isListening = false;
    isBAccessed = false;
    freq = 0;
    isABRaceDetected = false;
    isABRaceDetected = false;
  }

  public void startListening() {
    isListening = true;
  }

  public void stopListening() {
    isListening = false;
  }

  public boolean isListening() {
    return isListening;
  }

  public synchronized boolean isBAccessed() {
    return isBAccessed;
  }

  public synchronized void markBAccessed() {
    isBAccessed = true;
  }

  public synchronized void reInitialise() {
    isBAccessed = false;
    isListening = false;
  }

  public void incFreqOfAccesses() {
    freq++;
  }

  public boolean isFreqLimitReached() {
    return (freq >= 5);
  }

  public synchronized void markRaceDetected(boolean isAToB) {
    if(isAToB) isABRaceDetected = true;
    else isBARaceDetected = true;
  }

  public synchronized boolean isRaceDetected() {
    return (isABRaceDetected || isBARaceDetected);
  }
}
