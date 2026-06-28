from com.arm.debug.dtsl.configurations import ConfigurationBaseSDF
from com.arm.debug.dtsl.configurations import DTSLv1
from com.arm.debug.dtsl.components import FormatterMode
from com.arm.debug.dtsl.components import AHBAP
from com.arm.debug.dtsl.components import APB4AP
from com.arm.debug.dtsl.components import AXI5AP
from com.arm.debug.dtsl.components import Device
from com.arm.debug.dtsl.components import DeviceInfo
from com.arm.debug.dtsl.configurations.options import IIntegerOption
from com.arm.debug.dtsl.components import CSTMC
from com.arm.debug.dtsl.components import ETRTraceCapture
from com.arm.debug.dtsl.components import ETRCircularBufferOnlyTraceCapture
from com.arm.debug.dtsl.components import DSTREAMTraceCapture
from com.arm.debug.dtsl.components import DSTREAMSTStoredTraceCapture
from com.arm.debug.dtsl.components import DSTREAMPTStoreAndForwardTraceCapture
from com.arm.debug.dtsl.components import DSTREAMPTLiveStoredStreamingTraceCapture
from com.arm.debug.dtsl.components import CSCTI
from com.arm.debug.dtsl.components import ETMv4TraceSource
from com.arm.debug.dtsl.components import CSTPIU
from com.arm.debug.dtsl.components import CSATBReplicator
from com.arm.debug.dtsl.components import CATU
from com.arm.debug.dtsl.components import V7M_ITMTraceSource
from com.arm.debug.dtsl.components import STMTraceSource
from com.arm.debug.dtsl.configurations import TimestampInfo

coreNames_cortexM0PLUS = ["Secure Enclave Cortex-M0+"]
coreNames_cortexM55 = ["RTSS-HE Cortex-M55"]



class DtslScript(ConfigurationBaseSDF):
    @staticmethod
    def getOptionList():
        return [
            DTSLv1.tabSet("options", "Options", childOptions=
                [DTSLv1.tabPage("trace", "Trace Capture", childOptions=[
                    DTSLv1.integerOption('timestampFrequency', 'Timestamp frequency', defaultValue=25000000, isDynamic=False, description="This value will be used to set the Counter Base Frequency ID Register of the Timestamp generator.\nIt represents the number of ticks per second and is used to translate the timestamp value reported into a number of seconds.\nNote that changing this value may not result in a change in the observed frequency."),
                    DTSLv1.enumOption('traceCapture', 'Trace capture method', defaultValue="none",
                        values = [("none", "None"), ("SoC TMC", "System Memory Trace Buffer (SoC TMC/ETR)"), DtslScript.getOffChipTraceOption()]),
                ])]
                +[DTSLv1.tabPage("cortexM55", "Cortex-M55", childOptions=[
                    DTSLv1.booleanOption('coreTrace', 'Enable Cortex-M55 core trace', defaultValue=False,
                        childOptions = [
                            DTSLv1.booleanOption('Cortex_M55_0', 'Enable RTSS-HE Cortex-M55 trace', defaultValue=True),
                            DTSLv1.booleanOption('timestamp', "Enable ETM Timestamps", description="Controls the output of timestamps into the ETM output streams", defaultValue=True),
                            ETMv4TraceSource.cycleAccurateOption(DtslScript.getSourcesForCoreType("Cortex-M55")),
                        ]
                    ),
                ])]
                +[DTSLv1.tabPage("ETR", "ETR", childOptions=[
                    DTSLv1.booleanOption('etrBuffer0', 'Configure the system memory trace buffer to be used by the APSS TMC/ETR device', defaultValue=False,
                        childOptions = [
                            DTSLv1.integerOption('start', 'Start address',
                            description='Start address of the system memory trace buffer to be used by the APSS TMC/ETR device',
                            defaultValue=0x00100000,
                            display=IIntegerOption.DisplayFormat.HEX),
                            DTSLv1.integerOption('size', 'Size in bytes',
                            description='Size of the system memory trace buffer in bytes',
                            defaultValue=0x8000,
                            isDynamic=True,
                            display=IIntegerOption.DisplayFormat.HEX)
                        ]
                    ),
                    DTSLv1.booleanOption('etrBuffer1', 'Configure the system memory trace buffer to be used by the SoC TMC/ETR device', defaultValue=False,
                        childOptions = [
                            DTSLv1.integerOption('start', 'Start address',
                            description='Start address of the system memory trace buffer to be used by the SoC TMC/ETR device',
                            defaultValue=0x00100000,
                            display=IIntegerOption.DisplayFormat.HEX),
                            DTSLv1.integerOption('size', 'Size in bytes',
                            description='Size of the system memory trace buffer in bytes',
                            defaultValue=0x8000,
                            isDynamic=True,
                            display=IIntegerOption.DisplayFormat.HEX)
                        ]
                    ),
                ])]
                +[DTSLv1.tabPage("itm", "ITM", childOptions=[
                    DTSLv1.booleanOption('RTSS-HE ITM-M55', 'Enable RTSS-HE ITM-M55 trace', defaultValue=False),
                ])]
                +[DTSLv1.tabPage("stm", "STM", childOptions=[
                    DTSLv1.booleanOption('APSS STM', 'Enable APSS STM trace', defaultValue=False),
                ])]
                +[DTSLv1.tabPage("sync", "CTI Synchronization", childOptions=[
                    DTSLv1.infoElement("Select the cores to be cross-triggered (not needed for SMP connections)"),
                    DTSLv1.booleanOption(coreNames_cortexM0PLUS[0], coreNames_cortexM0PLUS[0], description="Add core to synchronization group", defaultValue=False, isDynamic=True),
                    DTSLv1.booleanOption(coreNames_cortexM55[0], coreNames_cortexM55[0], description="Add core to synchronization group", defaultValue=False, isDynamic=True),
                ])]
            )
        ]
    
    @staticmethod
    def getOffChipTraceOption():
        return ("DSTREAM", "DSTREAM 4GB Trace Buffer",
            DTSLv1.infoElement("dstream", "Off-Chip Trace", "",
                childOptions=[
                    DTSLv1.enumOption('tpiuPortWidth', 'TPIU Port Width', defaultValue="4",
                        values = [("1", "1 bit"), ("2", "2 bit"), ("3", "3 bit"), ("4", "4 bit")], isDynamic=False),
                ]
            )
        )
    
    def __init__(self, root):
        ConfigurationBaseSDF.__init__(self, root)
        
        self.discoverDevices()
    
    # +----------------------------+
    # | Target dependent functions |
    # +----------------------------+
    
    def discoverDevices(self):
        '''Find and create devices'''
        
        # MEMAP devices
        AHBAP(self, self.findDevice("Secure Enclave AHB-AP"), "Secure Enclave AHB-AP")
        APB4AP(self, self.findDevice("APSS APB-AP"), "APSS APB-AP")
        AXI5AP(self, self.findDevice("AXI-AP"), "AXI-AP", False)
        AHBAP(self, self.findDevice("RTSS-HE AHB-AP"), "RTSS-HE AHB-AP")
        
        # Trace start/stop CTIs
        CSCTI(self, self.findDevice("APSS CTI"), "APSS CTI")
        CSCTI(self, self.findDevice("SoC CTI"), "SoC CTI")
        
        # The ATB stream ID which will be assigned to trace sources.
        streamID = 2
        
        stm = STMTraceSource(self, self.findDevice("APSS STM"), streamID, "APSS STM")
        stm.setEnabled(False)
        streamID += 1
        
        self.cortexM0PLUScores = []
        # Ensure that macrocell StreamIDs are grouped such that they can be filtered by a programmable replicator.
        streamID += (0x10 - (streamID % 0x10))
        for coreName in (coreNames_cortexM0PLUS):
            # Create core
            coreDevice = Device(self, self.findDevice(coreName), coreName)
            deviceInfo = DeviceInfo("core", "Cortex-M0+")
            coreDevice.setDeviceInfo(deviceInfo)
            self.cortexM0PLUScores.append(coreDevice)
            self.addDeviceInterface(coreDevice)
            
            # Create CTI (if a CTI exists for this core)
            ctiName = self.getCTINameForCore(coreName)
            if not ctiName is None:
                CSCTI(self, self.findDevice(ctiName), ctiName)
                self.registerCoreForCrossSync(coreDevice)
            
        self.cortexM55cores = []
        # Ensure that macrocell StreamIDs are grouped such that they can be filtered by a programmable replicator.
        streamID += (0x10 - (streamID % 0x10))
        for coreName in (coreNames_cortexM55):
            # Create core
            coreDevice = Device(self, self.findDevice(coreName), coreName)
            deviceInfo = DeviceInfo("core", "Cortex-M55")
            coreDevice.setDeviceInfo(deviceInfo)
            self.cortexM55cores.append(coreDevice)
            self.addDeviceInterface(coreDevice)
            
            # Create CTI (if a CTI exists for this core)
            ctiName = self.getCTINameForCore(coreName)
            if not ctiName is None:
                CSCTI(self, self.findDevice(ctiName), ctiName)
                self.registerCoreForCrossSync(coreDevice)
            
            # Create Trace Macrocell (if a macrocell exists for this core - disabled by default - will enable with option)
            tmName = self.getTraceSourceNameForCore(coreName)
            if not tmName == None:
                tm = ETMv4TraceSource(self, self.findDevice(tmName), streamID, tmName)
                streamID += 2
                tm.setEnabled(False)
            
        tpiu = CSTPIU(self, self.findDevice("SoC TPIU"), "SoC TPIU")
        tpiu.setEnabled(False)
        tpiu.setFormatterMode(FormatterMode.CONTINUOUS)
        
        # Create and Configure Funnels
        self.createFunnel("APSS Funnel")
        self.createFunnel("Soc TPIU Funnel")
        
        # Replicators
        CSATBReplicator(self, self.findDevice("APSS Replicator"), "APSS Replicator")
        CSATBReplicator(self, self.findDevice("Soc TPIU Replicator"), "Soc TPIU Replicator")
        
        # CoreSight Address Translation Units
        CATU(self, self.findDevice("APSS CATU"), "APSS CATU")
        CATU(self, self.findDevice("SoC CATU"), "SoC CATU")
        
        # Assign low stream IDs to ITMs
        itmStreamID = 1
        
        itm = V7M_ITMTraceSource(self, self.findDevice("RTSS-HE ITM-M55"), itmStreamID, "RTSS-HE ITM-M55", self.getDeviceInterfaces().get("RTSS-HE Cortex-M55"))
        itm.setPortPrivileges(True, True, True, True)
        itm.setIsSetupByTarget(False)
        itm.setEnabled(False)
        itmStreamID += 1
        
    def createETRTraceCapture(self, deviceName, fullMemoryAccess, hasFormatter):
        if fullMemoryAccess:
            etrTrace = ETRTraceCapture(self, self.findDevice(deviceName), deviceName)
        else:
            etrTrace = ETRCircularBufferOnlyTraceCapture(self, self.findDevice(deviceName), deviceName, self.findMemoryDevice(deviceName))
        if hasFormatter:
            etrTrace.setFormatterMode(FormatterMode.CONTINUOUS)
        else:
            etrTrace.setFormatterMode(FormatterMode.BYPASS)
        self.addTraceCaptureInterface(etrTrace)
    
    def createDSTREAMTraceCapture(self, dstreamMode):
        self.DSTREAM = DSTREAMTraceCapture(self, dstreamMode)
        self.addTraceCaptureInterface(self.DSTREAM)
    
    def getDstreamOptionString(self, dstreamMode):
        return "dstream"
    
    def postConnect(self):
        ConfigurationBaseSDF.postConnect(self)
        
        if self.getOptions().getOption("options.trace.timestampFrequency"):
            freq = self.getOptionValue("options.trace.timestampFrequency")
            # Update the value so the trace decoder can access it
            tsInfo = TimestampInfo(freq)
            self.setTimestampInfo(tsInfo)
        
    
    # +--------------------------------+
    # | Callback functions for options |
    # +--------------------------------+
    
    def optionValuesChanged(self):
        '''Callback to update the configuration state after options are changed'''
        if not self.isConnected():
            self.setInitialOptions()
        
        self.updateDynamicOptions()
        
    def setInitialOptions(self):
        '''Set the initial options'''
        
        coreTraceEnabled = self.getOptionValue("options.cortexM55.coreTrace")
        for core in range(len(coreNames_cortexM55)):
            tmName = self.getTraceSourceNameForCore(coreNames_cortexM55[core])
            if tmName:
                coreTM = self.getDeviceInterface(tmName)
                thisCoreTraceEnabled = self.getOptionValue("options.cortexM55.coreTrace.Cortex_M55_%d" % core)
                enableSource = coreTraceEnabled and thisCoreTraceEnabled
                self.setTraceSourceEnabled(tmName, enableSource)
                coreTM.setTimestampingEnabled(self.getOptionValue("options.cortexM55.coreTrace.timestamp"))
        
        itmEnabled = self.getOptionValue("options.itm.RTSS-HE ITM-M55")
        self.setTraceSourceEnabled("RTSS-HE ITM-M55", itmEnabled)
        
        stmEnabled = self.getOptionValue("options.stm.APSS STM")
        self.setTraceSourceEnabled("APSS STM", stmEnabled)
        
        traceMode = self.getOptionValue("options.trace.traceCapture")
        if traceMode != "none":
            # ETR Capture
            if traceMode == "APSS TMC":
                self.createETRTraceCapture("APSS TMC", True, True)
            if traceMode == "SoC TMC":
                self.createETRTraceCapture("SoC TMC", True, True)
            # DSTREAM
            if traceMode in ["DSTREAM", "DSTREAM_PT_Store_and_Forward", "DSTREAM_PT_StreamingTrace"]:
                self.createDSTREAMTraceCapture(traceMode)
                if self.getOptions().getOption("options.trace.traceCapture." + self.getDstreamOptionString(traceMode) + ".tpiuPortWidth"):
                    self.setPortWidth(int(self.getOptionValue("options.trace.traceCapture." + self.getDstreamOptionString(traceMode) + ".tpiuPortWidth")))
                
                if self.getOptions().getOption("options.trace.traceCapture." + self.getDstreamOptionString(traceMode) + ".traceBufferSize"):
                    self.setTraceBufferSize(self.getOptionValue("options.trace.traceCapture." + self.getDstreamOptionString(traceMode) + ".traceBufferSize"))
                
            self.enableTraceCapture(traceMode)
            self.configureTraceCapture(traceMode)
            
    def updateDynamicOptions(self):
        '''Update the dynamic options'''
        
        # Set up the ETR 0 buffer
        configureETRBuffer = self.getOptionValue("options.ETR.etrBuffer0") and (self.getTraceCaptureInterfaces().get("APSS TMC") is not None)
        if configureETRBuffer:
            etr = self.getTraceCaptureInterfaces()["APSS TMC"]
            etr.setBaseAddress(self.getOptionValue("options.ETR.etrBuffer0.start"))
            etr.setTraceBufferSize(self.getOptionValue("options.ETR.etrBuffer0.size"))
            
        # Set up the ETR 1 buffer
        configureETRBuffer = self.getOptionValue("options.ETR.etrBuffer1") and (self.getTraceCaptureInterfaces().get("SoC TMC") is not None)
        if configureETRBuffer:
            etr = self.getTraceCaptureInterfaces()["SoC TMC"]
            etr.setBaseAddress(self.getOptionValue("options.ETR.etrBuffer1.start"))
            etr.setTraceBufferSize(self.getOptionValue("options.ETR.etrBuffer1.size"))
            
        for core in range(len(self.cortexM0PLUScores)):
            enable = self.getOptionValue('options.sync.%s' % coreNames_cortexM0PLUS[core])
            self.setCrossSyncEnabled(self.cortexM0PLUScores[core], enable)
        
        for core in range(len(self.cortexM55cores)):
            enable = self.getOptionValue('options.sync.%s' % coreNames_cortexM55[core])
            self.setCrossSyncEnabled(self.cortexM55cores[core], enable)
        
    @staticmethod
    def getSourcesForCoreType(coreType):
        '''Get the Trace Sources for a given coreType
           Use parameter-binding to ensure that the correct Sources
           are returned for the core type passed only'''
        def getSources(self):
            return self.getTraceSourcesForCoreType(coreType)
        return getSources
    
class DtslScript_DSTREAM_ST(DtslScript):
    @staticmethod
    def getOptionList():
        return [
            DTSLv1.tabSet("options", "Options", childOptions=
                [DTSLv1.tabPage("trace", "Trace Capture", childOptions=[
                    DTSLv1.integerOption('timestampFrequency', 'Timestamp frequency', defaultValue=25000000, isDynamic=False, description="This value will be used to set the Counter Base Frequency ID Register of the Timestamp generator.\nIt represents the number of ticks per second and is used to translate the timestamp value reported into a number of seconds.\nNote that changing this value may not result in a change in the observed frequency."),
                    DTSLv1.enumOption('traceCapture', 'Trace capture method', defaultValue="none",
                        values = [("none", "None"), ("SoC TMC", "System Memory Trace Buffer (SoC TMC/ETR)"), DtslScript_DSTREAM_ST.getOffChipTraceOption()]),
                ])]
                +[DTSLv1.tabPage("cortexM55", "Cortex-M55", childOptions=[
                    DTSLv1.booleanOption('coreTrace', 'Enable Cortex-M55 core trace', defaultValue=False,
                        childOptions = [
                            DTSLv1.booleanOption('Cortex_M55_0', 'Enable RTSS-HE Cortex-M55 trace', defaultValue=True),
                            DTSLv1.booleanOption('timestamp', "Enable ETM Timestamps", description="Controls the output of timestamps into the ETM output streams", defaultValue=True),
                            ETMv4TraceSource.cycleAccurateOption(DtslScript.getSourcesForCoreType("Cortex-M55")),
                        ]
                    ),
                ])]
                +[DTSLv1.tabPage("ETR", "ETR", childOptions=[
                    DTSLv1.booleanOption('etrBuffer0', 'Configure the system memory trace buffer to be used by the APSS TMC/ETR device', defaultValue=False,
                        childOptions = [
                            DTSLv1.integerOption('start', 'Start address',
                            description='Start address of the system memory trace buffer to be used by the APSS TMC/ETR device',
                            defaultValue=0x00100000,
                            display=IIntegerOption.DisplayFormat.HEX),
                            DTSLv1.integerOption('size', 'Size in bytes',
                            description='Size of the system memory trace buffer in bytes',
                            defaultValue=0x8000,
                            isDynamic=True,
                            display=IIntegerOption.DisplayFormat.HEX)
                        ]
                    ),
                    DTSLv1.booleanOption('etrBuffer1', 'Configure the system memory trace buffer to be used by the SoC TMC/ETR device', defaultValue=False,
                        childOptions = [
                            DTSLv1.integerOption('start', 'Start address',
                            description='Start address of the system memory trace buffer to be used by the SoC TMC/ETR device',
                            defaultValue=0x00100000,
                            display=IIntegerOption.DisplayFormat.HEX),
                            DTSLv1.integerOption('size', 'Size in bytes',
                            description='Size of the system memory trace buffer in bytes',
                            defaultValue=0x8000,
                            isDynamic=True,
                            display=IIntegerOption.DisplayFormat.HEX)
                        ]
                    ),
                ])]
                +[DTSLv1.tabPage("itm", "ITM", childOptions=[
                    DTSLv1.booleanOption('RTSS-HE ITM-M55', 'Enable RTSS-HE ITM-M55 trace', defaultValue=False),
                ])]
                +[DTSLv1.tabPage("stm", "STM", childOptions=[
                    DTSLv1.booleanOption('APSS STM', 'Enable APSS STM trace', defaultValue=False),
                ])]
                +[DTSLv1.tabPage("sync", "CTI Synchronization", childOptions=[
                    DTSLv1.infoElement("Select the cores to be cross-triggered (not needed for SMP connections)"),
                    DTSLv1.booleanOption(coreNames_cortexM0PLUS[0], coreNames_cortexM0PLUS[0], description="Add core to synchronization group", defaultValue=False, isDynamic=True),
                    DTSLv1.booleanOption(coreNames_cortexM55[0], coreNames_cortexM55[0], description="Add core to synchronization group", defaultValue=False, isDynamic=True),
                ])]
            )
        ]
    
    @staticmethod
    def getOffChipTraceOption():
        return ("DSTREAM", "DSTREAM-ST Streaming Trace",
            DTSLv1.infoElement("dstream", "Off-Chip Trace", "",
                childOptions=[
                    DTSLv1.enumOption('tpiuPortWidth', 'TPIU Port Width', defaultValue="4",
                        values = [("1", "1 bit"), ("2", "2 bit"), ("4", "4 bit")], isDynamic=False),
                    DTSLv1.enumOption('traceBufferSize', 'Host Trace Buffer Size', defaultValue="4GB",
                        values = [("64MB", "64MB"), ("128MB", "128MB"), ("256MB", "256MB"), ("512MB", "512MB"), ("1GB", "1GB"), ("2GB", "2GB"), ("4GB", "4GB"), ("8GB", "8GB"), ("16GB", "16GB"), ("32GB", "32GB"), ("64GB", "64GB"), ("128GB", "128GB")], isDynamic=False)
                ]
            )
        )
    
    def createDSTREAMTraceCapture(self, dstreamMode):
        self.DSTREAM = DSTREAMSTStoredTraceCapture(self, dstreamMode)
        self.addTraceCaptureInterface(self.DSTREAM)
        self.addStreamTraceCaptureInterface(self.DSTREAM)
    

class DtslScript_DSTREAM_PT(DtslScript):
    @staticmethod
    def getOptionList():
        return [
            DTSLv1.tabSet("options", "Options", childOptions=
                [DTSLv1.tabPage("trace", "Trace Capture", childOptions=[
                    DTSLv1.integerOption('timestampFrequency', 'Timestamp frequency', defaultValue=25000000, isDynamic=False, description="This value will be used to set the Counter Base Frequency ID Register of the Timestamp generator.\nIt represents the number of ticks per second and is used to translate the timestamp value reported into a number of seconds.\nNote that changing this value may not result in a change in the observed frequency."),
                    DTSLv1.enumOption('traceCapture', 'Trace capture method', defaultValue="none",
                        values = [("none", "None"), ("SoC TMC", "System Memory Trace Buffer (SoC TMC/ETR)"), DtslScript_DSTREAM_PT.getStoreAndForwardOptions(), DtslScript_DSTREAM_PT.getStreamingTraceOptions()]),
                ])]
                +[DTSLv1.tabPage("cortexM55", "Cortex-M55", childOptions=[
                    DTSLv1.booleanOption('coreTrace', 'Enable Cortex-M55 core trace', defaultValue=False,
                        childOptions = [
                            DTSLv1.booleanOption('Cortex_M55_0', 'Enable RTSS-HE Cortex-M55 trace', defaultValue=True),
                            DTSLv1.booleanOption('timestamp', "Enable ETM Timestamps", description="Controls the output of timestamps into the ETM output streams", defaultValue=True),
                            ETMv4TraceSource.cycleAccurateOption(DtslScript.getSourcesForCoreType("Cortex-M55")),
                        ]
                    ),
                ])]
                +[DTSLv1.tabPage("ETR", "ETR", childOptions=[
                    DTSLv1.booleanOption('etrBuffer0', 'Configure the system memory trace buffer to be used by the APSS TMC/ETR device', defaultValue=False,
                        childOptions = [
                            DTSLv1.integerOption('start', 'Start address',
                            description='Start address of the system memory trace buffer to be used by the APSS TMC/ETR device',
                            defaultValue=0x00100000,
                            display=IIntegerOption.DisplayFormat.HEX),
                            DTSLv1.integerOption('size', 'Size in bytes',
                            description='Size of the system memory trace buffer in bytes',
                            defaultValue=0x8000,
                            isDynamic=True,
                            display=IIntegerOption.DisplayFormat.HEX)
                        ]
                    ),
                    DTSLv1.booleanOption('etrBuffer1', 'Configure the system memory trace buffer to be used by the SoC TMC/ETR device', defaultValue=False,
                        childOptions = [
                            DTSLv1.integerOption('start', 'Start address',
                            description='Start address of the system memory trace buffer to be used by the SoC TMC/ETR device',
                            defaultValue=0x00100000,
                            display=IIntegerOption.DisplayFormat.HEX),
                            DTSLv1.integerOption('size', 'Size in bytes',
                            description='Size of the system memory trace buffer in bytes',
                            defaultValue=0x8000,
                            isDynamic=True,
                            display=IIntegerOption.DisplayFormat.HEX)
                        ]
                    ),
                ])]
                +[DTSLv1.tabPage("itm", "ITM", childOptions=[
                    DTSLv1.booleanOption('RTSS-HE ITM-M55', 'Enable RTSS-HE ITM-M55 trace', defaultValue=False),
                ])]
                +[DTSLv1.tabPage("stm", "STM", childOptions=[
                    DTSLv1.booleanOption('APSS STM', 'Enable APSS STM trace', defaultValue=False),
                ])]
                +[DTSLv1.tabPage("sync", "CTI Synchronization", childOptions=[
                    DTSLv1.infoElement("Select the cores to be cross-triggered (not needed for SMP connections)"),
                    DTSLv1.booleanOption(coreNames_cortexM0PLUS[0], coreNames_cortexM0PLUS[0], description="Add core to synchronization group", defaultValue=False, isDynamic=True),
                    DTSLv1.booleanOption(coreNames_cortexM55[0], coreNames_cortexM55[0], description="Add core to synchronization group", defaultValue=False, isDynamic=True),
                ])]
            )
        ]
    
    @staticmethod
    def getStoreAndForwardOptions():
        return ("DSTREAM_PT_Store_and_Forward", "DSTREAM-PT 8GB Trace Buffer",
            DTSLv1.infoElement("dpt_storeandforward", "Off-Chip Trace", "",
                childOptions=[
                    DTSLv1.enumOption('tpiuPortWidth', 'TPIU Port Width', defaultValue="4",
                        values = [("1", "1 bit"), ("2", "2 bit"), ("3", "3 bit"), ("4", "4 bit")], isDynamic=False),
                ]
            )
        )
    
    @staticmethod
    def getStreamingTraceOptions():
        return ("DSTREAM_PT_StreamingTrace", "DSTREAM-PT Streaming Trace",
            DTSLv1.infoElement("dpt_streamingtrace", "Off-Chip Trace", "",
                childOptions=[
                    DTSLv1.enumOption('tpiuPortWidth', 'TPIU Port Width', defaultValue="4",
                        values = [("1", "1 bit"), ("2", "2 bit"), ("3", "3 bit"), ("4", "4 bit")], isDynamic=False),
                    DTSLv1.enumOption('traceBufferSize', 'Host Trace Buffer Size', defaultValue="4GB",
                        values = [("64MB", "64MB"), ("128MB", "128MB"), ("256MB", "256MB"), ("512MB", "512MB"), ("1GB", "1GB"), ("2GB", "2GB"), ("4GB", "4GB"), ("8GB", "8GB"), ("16GB", "16GB"), ("32GB", "32GB"), ("64GB", "64GB"), ("128GB", "128GB")], isDynamic=False)
                ]
            )
        )
    
    def createDSTREAMTraceCapture(self, dstreamMode):
        if dstreamMode == "DSTREAM_PT_Store_and_Forward":
            self.DSTREAM = DSTREAMPTStoreAndForwardTraceCapture(self, "DSTREAM_PT_Store_and_Forward")
        elif dstreamMode == "DSTREAM_PT_StreamingTrace":
            self.DSTREAM = DSTREAMPTLiveStoredStreamingTraceCapture(self, "DSTREAM_PT_StreamingTrace")
        self.addTraceCaptureInterface(self.DSTREAM)
        self.addStreamTraceCaptureInterface(self.DSTREAM)
    
    def getDstreamOptionString(self, dstreamMode):
        if dstreamMode == "DSTREAM_PT_Store_and_Forward":
            return "dpt_storeandforward"
        if dstreamMode == "DSTREAM_PT_StreamingTrace":
            return "dpt_streamingtrace"
    

class DtslScript_DebugAndOnChipTrace(DtslScript):
    @staticmethod
    def getOptionList():
        return [
            DTSLv1.tabSet("options", "Options", childOptions=
                [DTSLv1.tabPage("trace", "Trace Capture", childOptions=[
                    DTSLv1.integerOption('timestampFrequency', 'Timestamp frequency', defaultValue=25000000, isDynamic=False, description="This value will be used to set the Counter Base Frequency ID Register of the Timestamp generator.\nIt represents the number of ticks per second and is used to translate the timestamp value reported into a number of seconds.\nNote that changing this value may not result in a change in the observed frequency."),
                    DTSLv1.enumOption('traceCapture', 'Trace capture method', defaultValue="none",
                        values = [("none", "None"), ("SoC TMC", "System Memory Trace Buffer (SoC TMC/ETR)")]),
                ])]
                +[DTSLv1.tabPage("cortexM55", "Cortex-M55", childOptions=[
                    DTSLv1.booleanOption('coreTrace', 'Enable Cortex-M55 core trace', defaultValue=False,
                        childOptions = [
                            DTSLv1.booleanOption('Cortex_M55_0', 'Enable RTSS-HE Cortex-M55 trace', defaultValue=True),
                            DTSLv1.booleanOption('timestamp', "Enable ETM Timestamps", description="Controls the output of timestamps into the ETM output streams", defaultValue=True),
                            ETMv4TraceSource.cycleAccurateOption(DtslScript.getSourcesForCoreType("Cortex-M55")),
                        ]
                    ),
                ])]
                +[DTSLv1.tabPage("ETR", "ETR", childOptions=[
                    DTSLv1.booleanOption('etrBuffer0', 'Configure the system memory trace buffer to be used by the APSS TMC/ETR device', defaultValue=False,
                        childOptions = [
                            DTSLv1.integerOption('start', 'Start address',
                            description='Start address of the system memory trace buffer to be used by the APSS TMC/ETR device',
                            defaultValue=0x00100000,
                            display=IIntegerOption.DisplayFormat.HEX),
                            DTSLv1.integerOption('size', 'Size in bytes',
                            description='Size of the system memory trace buffer in bytes',
                            defaultValue=0x8000,
                            isDynamic=True,
                            display=IIntegerOption.DisplayFormat.HEX)
                        ]
                    ),
                    DTSLv1.booleanOption('etrBuffer1', 'Configure the system memory trace buffer to be used by the SoC TMC/ETR device', defaultValue=False,
                        childOptions = [
                            DTSLv1.integerOption('start', 'Start address',
                            description='Start address of the system memory trace buffer to be used by the SoC TMC/ETR device',
                            defaultValue=0x00100000,
                            display=IIntegerOption.DisplayFormat.HEX),
                            DTSLv1.integerOption('size', 'Size in bytes',
                            description='Size of the system memory trace buffer in bytes',
                            defaultValue=0x8000,
                            isDynamic=True,
                            display=IIntegerOption.DisplayFormat.HEX)
                        ]
                    ),
                ])]
                +[DTSLv1.tabPage("itm", "ITM", childOptions=[
                    DTSLv1.booleanOption('RTSS-HE ITM-M55', 'Enable RTSS-HE ITM-M55 trace', defaultValue=False),
                ])]
                +[DTSLv1.tabPage("stm", "STM", childOptions=[
                    DTSLv1.booleanOption('APSS STM', 'Enable APSS STM trace', defaultValue=False),
                ])]
                +[DTSLv1.tabPage("sync", "CTI Synchronization", childOptions=[
                    DTSLv1.infoElement("Select the cores to be cross-triggered (not needed for SMP connections)"),
                    DTSLv1.booleanOption(coreNames_cortexM0PLUS[0], coreNames_cortexM0PLUS[0], description="Add core to synchronization group", defaultValue=False, isDynamic=True),
                    DTSLv1.booleanOption(coreNames_cortexM55[0], coreNames_cortexM55[0], description="Add core to synchronization group", defaultValue=False, isDynamic=True),
                ])]
            )
        ]

