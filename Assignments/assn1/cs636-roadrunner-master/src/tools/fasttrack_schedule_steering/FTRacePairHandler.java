package tools.fasttrack_schedule_steering;

import acme.util.Assert;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import rr.meta.SourceLocation;

public class FTRacePairHandler {

  private HashSet<String> sourceLocationsToTrack = new HashSet<String>();
  private HashMap<String, FTMetaDataForPerturbationTracking> metaDataForA =
    new HashMap<String, FTMetaDataForPerturbationTracking>();

  private HashMap<String, String> sourceLocationsBToA =
    new HashMap<String, String>();

  public FTRacePairHandler (final String filename) {

    File file = new File(filename);
    try {
      FileReader fr = new FileReader(file);
      BufferedReader br = new BufferedReader(fr);

      String line;
      while ((line = br.readLine()) != null) {
        String[] racePair = line.split("@",2);

        String a = racePair[0].substring(racePair[0].lastIndexOf('('));
        String b = racePair[1].substring(racePair[1].lastIndexOf('('));

        sourceLocationsToTrack.add(a);
        sourceLocationsToTrack.add(b);
        sourceLocationsBToA.put(b, a);

        metaDataForA
          .put(a, new FTMetaDataForPerturbationTracking(b));
      }
    } catch (IOException e) {
        System.out.println("Error: File NOT found for RacePair");
        Assert.assertTrue(false);
    }
  }

  public Boolean isSourceLocationValid(String sl) {
    return sourceLocationsToTrack.contains(sl);
  }

  public String getAOfRacePairFromB(String b) {
    String a = sourceLocationsBToA.get(b);
    return a;
  }

  public FTMetaDataForPerturbationTracking getMetaDataForA(String a) {
    return metaDataForA.get(a);
  }

  public String generateSourceLocation(SourceLocation sl) {
    String file = sl.getFile();
    String sourceLocation =
      "(" + file.substring(file.lastIndexOf('/') + 1) + "):" +
        sl.getLine() +
        (sl.getOffset() > -1 ? ":" + sl.getOffset() : "");
    return sourceLocation;
  }
}

