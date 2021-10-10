# CompiledRosettaScripts

## The plan

Example:
```c++
ScoreFunction main( pose, args ){

  ScoreFunction sfxn1 = ScoreFunction( weights="beta_cst", symmetric="1" );

  ScoreFunction sfxn2 = lib::ref2015;

  Mover design = FastDesign( task_operations=[ lib::include_current, lib::ex1ex2 ] );

  if( args::use_ref2015 ){
    design.scorefxn = sfxn2;
  } else {
    design.scorefxn = sfxn1;
  }

  design.apply( pose );
  
  SimpleMetric refscore = lib::sf2sm( sfxn2 );
  
  SimpleMetric rescore_shallow_copy = refscore;
  SimpleMetric rescore_deep_copy = &refscore;
  rescore_deep_copy.scoretype = "fa_rep";
  
  ResidueSelector chainA = Chain( chains="A" );
  
  //# pound-sign comments are included in the XML ahead of the next object being modified or applied
  refscore.residue_selector = chainA;
  
  refscore.apply( pose );
  rescore_shallow_copy.apply( pose );
  
  //# another example of a comment
  rescore_deep_copy.apply( pose );
  
  return design.scorefxn;
}
```

Running with `./compile script.txt --use_ref2015 True` compiles To:
```xml
<ROSETTASCRIPTS>
    <SCOREFXNS>        
        <ScoreFunction name="sfxn2" weights="ref2015"/>
    </SCOREFXNS>
    
    <RESIDUE_SELECTORS>
        <Chain name="chainA" chains="A"/>
    </RESIDUE_SELECTORS>
    
    <TASKOPERATIONS>
        <IncludeCurrent name="lib_include_current"/>
        <ExtraChi name="lib_ex1ex2" ex1="true" ex2="true" />
    </TASKOPERATIONS>
    
    <SIMPLE_METRICS>
      
        <!-- pound-sign comments are included in the XML ahead of the next object being modified -->
        <TotalEnergyMetric name="refscore" scorefxn="sfxn2" residue_selector="chainA"/>
      
        <TotalEnergyMetric name="rescore_shallow_copy" scorefxn="sfxn2"/>
        <TotalEnergyMetric name="rescore_deep_copy" scorefxn="sfxn2" residue_selector="chainA" scoretype="fa_rep"/>
    </SIMPLE_METRICS>

    <MOVERS>
        <FastDesign name="design" scorefxn="sfxn2" task_operations="lib_include_current,lib_ex1ex2"/>
    </MOVERS>
  
    <PROTOCOLS>
      <Add mover="design"/>
      <Add metrics="refscore">
      <Add metrics="rescore_shallow_copy">
        
      <!-- another example of a comment -->
      <Add metrics="rescore_deep_copy">
    </PROTOCOLS>
        
    <OUTPUT scorefxn="sfxn2"/>
</ROSETTASCRIPTS>
```

Note the compiler is able to completely exclude:
```xml
<ScoreFunction name="sfxn1" weights="beta_cst" symmetric="1">
    <Reweight scoretype="coordinate_constraint" weight="0.5" />
</ScoreFunction>
```

### More plans

#### Functions
```c++
ScoreFunction enable_csts( SCOREFXN sfxn ){
  ScoreFunction sfxn2 = sfxn;
  sfxn2.Reweight( scoretype="coordinate_constraint", weight="0" );
  sfxn2.Reweight( scoretype="sequence_constraint", weight="0" );
  return sfxn2;
}

ScoreFunction main( pose, args ){
  Mover design = FastDesign( task_operations=[ lib::include_current, lib::ex1ex2 ], scorefxn=lib::ref2015_cst );
  design.apply( pose );
  
  return disable_csts( design.scorefxn );
}
```

Creates:

```xml
<ROSETTASCRIPTS>
    <SCOREFXNS>        
        <ScoreFunction name="sfxn" weights="ref2015_cst"/>
      
        <ScoreFunction name="output_sfxn" weights="ref2015_cst">
            <Reweight scoretype="coordinate_constraint" weight="0" />
            <Reweight scoretype="sequence_constraint" weight="0" />
        </ScoreFunction>
    </SCOREFXNS>
    
    <TASKOPERATIONS>
        <IncludeCurrent name="lib_include_current"/>
        <ExtraChi name="lib_ex1ex2" ex1="true" ex2="true" />
    </TASKOPERATIONS>

    <MOVERS>
        <FastDesign name="design" scorefxn="sfxn" task_operations="lib_include_current,lib_ex1ex2"/>
    </MOVERS>
  
    <PROTOCOLS>
      <Add mover="design"/>
    </PROTOCOLS>
        
    <OUTPUT scorefxn="output_sfxn"/>
</ROSETTASCRIPTS>
```

#### Warnings
```c++
ScoreFunction enable_csts( ScoreFunction sfxn ){
  ScoreFunction sfxn2 = sfxn;
  sfxn2.Reweight( scoretype="coordinate_constraint", weight="0" );
  sfxn2.Reweight( scoretype="sequence_constraint", weight="0" );
  return sfxn2;
}

ScoreFunction main( pose, args ){
  Mover design = FastDesign( task_operations=[ lib::include_current, lib::ex1ex2 ], scorefxn=lib::ref2015_cst );
  return disable_csts( design.scorefxn );
}
```

`WARNING: unused object 'design'`

#### More Conversions
```c++
ScoreFunction main( pose, args ){
  SimpleMetric refscore = lib::sf2sm( lib::ref2015 );
  Filter f = lib::sm2f( sf=refscore, cutoff="0.1", comparison_type="lt" );
  f.apply( pose );
}
```

Creates
```xml
<ROSETTASCRIPTS>
    <SCOREFXNS>        
        <ScoreFunction name="lib_ref2015" weights="ref2015"/>
    </SCOREFXNS>
    
    <SIMPLE_METRICS>
        <TotalEnergyMetric name="refscore" scorefxn="lib_ref2015"/>
    </SIMPLE_METRICS>

    <FILTERS>
        <SimpleMetricFilter name="f" metric="refscore" cutoff="0.1" comparison_type="lt"/>
    </FILTERS>
  
    <PROTOCOLS>
      <Add filter="f"/>
    </PROTOCOLS>
        
    <OUTPUT scorefxn="commandline"/>
</ROSETTASCRIPTS>
```
