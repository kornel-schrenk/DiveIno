## Description

This software converts the dive profile log files from DiveIno dive computer format into UDDF or UDCF format.

It is written in [Java](https://java.com/en/) with the [JavaFX Rich Client Platform](http://www.oracle.com/technetwork/java/javase/overview/javafx-overview-2158620.html).

## Installation ###

### Java

1. Install [OpenJDK](http://openjdk.java.net/install/) or [Oracle Java SE](http://www.oracle.com/technetwork/java/javase/downloads/index.html) - JDK 8 is required!
2. Install [Apache Maven](https://maven.apache.org/install.html)
3. Install [Apache Ant](http://ant.apache.org/manual/index.html)
4. Install [Inno Setup](http://www.jrsoftware.org/isdl.php) on Windows
5. Open a console and navigate to the root of your cloned repository
6. Execute _mvn compile_
7. Go to the **build** directory
8. Execute _ant_
9. Go to the deploy directory
10. Type _java -jar DiveInoConverter.jar_

Once you created the _DiveInoConverter.jar_ file, double click on it and the application will start.

## Blog posts

* [DiveIno logfile format](http://www.diveino.hu/2016/05/27/LogfileFormat/)
* [Migrate DiveIno files into json format](http://www.diveino.hu/2016/12/20/JsonMigration/)
* Develop DiveIno Converter

## References

* [DiveIno](http://www.diveino.hu)
* [UDCF - Universal Dive Computer Format](http://www.streit.cc/extern/udcf/udcf_doc_eng.html#toc2)
* [UDDF - Universal Dive Data Format](http://www.uddf.org/)
* [JavaFX deployment](http://code.makery.ch/library/javafx-8-tutorial/part7/)
