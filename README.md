# CompiledRosettaScripts

Example:
```
main( pose, args ){

  SCOREFXN sfxn1 = ScoreFunction( weights="beta_cst", symmetric="1" );
  sfxn1.Reweight( scoretype="coordinate_constraint", weight="0.5" )

  SCOREFXN sfxn2 = lib::ref2015;

  MOVER design = FastDesign( task_operations=[ lib::include_current, lib::ex1ex2 ] );

  if( args::use_ref2015 ){
    design.scorefxn = sfxn2;
  } else {
    design.scorefxn = sfxn1;
  }

  design.apply( pose );
  
  # dollar-sign applies an implicit wrapper, if it is available
  SIMPLE_METRIC refscore = $sfxn2;
  
  SIMPLE_METRIC rescore_shallow_copy = refscore;
  SIMPLE_METRIC rescore_deep_copy = &refscore;
  rescore_deep_copy.scoretype = "fa_rep";
  
  RESIDUE_SELECTOR chainA = Chain( chains="A" );
  
  refscore.residue_selector = chainA;
  
  refscore.apply( pose );
  rescore_shallow_copy.apply( pose );
  rescore_deep_copy.apply( pose );
  
  OUTPUT design.scorefxn;
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
        <TotalEnergyMetric name="refscore" residue_selector="chainA"/>
        <TotalEnergyMetric name="rescore_shallow_copy"/>
        <TotalEnergyMetric name="rescore_deep_copy" residue_selector="chainA" scoretype="fa_rep"/>
    </SIMPLE_METRICS>

    <MOVERS>
        <FastDesign name="design" scorefxn="sfxn2" task_operations="lib_include_current,lib_ex1ex2"/>
    </MOVERS>
  
    <PROTOCOLS>
      <Add mover="design"/>
      <Add metrics="refscore">
      <Add metrics="rescore_shallow_copy">
      <Add metrics="rescore_deep_copy">
    </PROTOCOLS>
        
    <OUTPUT scorefxn="sfxn2"/>
</ROSETTASCRIPTS>
```

Note the compiler is able to completely exclude:
```xml
<ScoreFunction name="sfxn1" weights="sfxn1" symmetric="1">
    <Reweight scoretype="coordinate_constraint" weight="0.5" />
</ScoreFunction>
```
