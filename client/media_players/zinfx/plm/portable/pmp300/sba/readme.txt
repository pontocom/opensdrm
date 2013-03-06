
        RIO utility v1.07 - The Snowblind Alliance (c) 1999
        ---------------------------------------------------

        Files
        -----
        README          This file
        CREDITS         Credits for contributors to this version
        rio.txt         Protocol information
        gpl.txt         Copy of licence

        std.h           standard defines
        binary.h        defines for binary numbers from 0-255
        rioioctl.h      ioctl constants used for WinNT version
        iopl32.h        low-level port i/o function declarations for OS/2
        rio.h           rio class header
        rio.cpp         rio class
        app.cpp         utility which uses rio class

        io.obj          low-level port i/o functions for OS/2

        os2.def         linker def file for OS/2

        playlist.txt    example playlist file

        makefile        linux makefile
        os2.mak         IVMVAC+ v3.0 makefile for OS/2 v4
        winnt.mak       MSVC++ v5.0 makefile for WinNT v4
        win9x.mak       MSVC++ v5.0 makefile for Win9x
        dos.mak         Borland Turbo C++ v1.01 for DOS

        Overview
        --------
        'rio' is a simple command line utility which allows
        you to display and manage files on your Diamond Rio
        portable music player.

        At present the Rio utility supports the Diamond Rio PMP300
        with 32M internal flash memory.
        Additional external flash memory is now supported thanks to
        the work done by Guido March (march@pc40.mpier.uni-frankfurt.de).

        command line switches available :-

        -d  display directory
        -iy initialize with check for bad blocks
        -in initialize without check for bad blocks
        -x  perform operations on external flash ram
        -u  specify file(s) to upload
        -g  specify file to download
        -f  specify text based playlist file which contains files to be upload
        -z  specify file to delete
        -za delete all files
        -o  specify new playlist order in quotes
        -p  specify parallel port base IO address, default=0x378
        -v  enable verbose mode
        -di specify initialization delay
        -dt specify tx delay
        -dr specify rx delay

        examples...

        ; display directory using parallel port at 0x278
                rio -p 0x278 -d
        ; initialize (with bad block check) and upload files
                rio -iy -u *.mp3
        ; delete existing files and upload playlist onto external flash ram
                rio -za -f playlist.txt -x
        ; initialize, upload files in playlist and then display directory
                rio -d -in -f playlist.txt
        ; download file then delete it and finally display directory
                rio -d -g mp3Files/song.mp3 -z song.mp3
        ; reverse playlist order and display directory, also change rx iodelay
                rio -o "5 4 3 2 1" -d -dr 4

        Initialization
        --------------
        Initialization should only be performed on new or corrupt cards.
        You can initialize with bad block checking enabled (-iy) which
        will test all blocks on the internal or external flash ram (which
        ever is selected) and note all bad blocks. This process will
        take some time (upto 20 minutes on slow machines).
        You can also initialize with bad block checking disabled (-in)
        which is much quicker but all blocks (even bad blocks) will
        be used.

        If you want to delete all files on the device, the (-za) option
        should be used instead of using the initialization options
        mentioned above.

        Building and running
        --------------------

                Linux, GCC
                ----------
                To build and run the application you must be
                logged on as root, then use 'make'.
                (rio utility has been built and tested
                using gcc compiler v2.7.2.3 under Linux v2.0.34)

                Windows NT v4.0, Visual C++ v5
                ------------------------------
                Use 'nmake /fwinnt.mak'.
                (rio utility has been built and tested using
                MS Visual C++ compiler v5 under
                Windows Workstation NT 4.0)

                Windows 95/98, Visual C++ v5
                ----------------------------
                Use 'nmake /fwin9x.mak'.
                (rio utility has been built and tested
                using MS Visual C++ compiler v5 under Windows 98)

                OS/2 v4.0, VisualAge C++ v3
                ------------------------------
                Use 'nmake /f os2.mak'.
                (rio utility has been built and tested using
                IBM VisualAge C++ compiler v3 under OS/2 v4.0)

                DOS, Borland Turbo C++ v1.01
                ----------------------------
                Use 'make -fdos.mak'
                (rio utility has been built and tested
                using Borland Turbo C++ v1.01 under DOS v6.22 and
                Windows 98).

        Known Bugs
        ----------
        Because of the difference in timings between different machines
        it is possible that you may experience problems when downloading
        and displaying the directory.
        If you encounter this problem try one or both of the following...

                Try increasing one or more of the command line
                delay parameters (-di, -dt, -dr). Type 'rio' without
                any command line parameters to display the default settings.

                Use the utility when the device is powered on as this
                seems to improve IO performance.

        Bug Reports
        -----------
        If you encounter any problems using this utility please send
        e-mail outlining the problem along with details about your
        Rio (memory installed, memory remaining and tracks loaded) to...

                sba@tessier-ashpool.cix.co.uk

        Licence Terms
        -------------

        This software is released under the terms of the GPL. Please see
        the file gpl.txt for further information.
