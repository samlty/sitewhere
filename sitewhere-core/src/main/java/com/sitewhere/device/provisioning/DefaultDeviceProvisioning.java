/*
 * DefaultDeviceProvisioning.java 
 * --------------------------------------------------------------------------------------
 * Copyright (c) Reveal Technologies, LLC. All rights reserved. http://www.reveal-tech.com
 *
 * The software in this package is published under the terms of the CPAL v1.0
 * license, a copy of which has been included with this distribution in the
 * LICENSE.txt file.
 */
package com.sitewhere.device.provisioning;

import java.util.ArrayList;
import java.util.List;

import org.apache.log4j.Logger;

import com.sitewhere.spi.SiteWhereException;
import com.sitewhere.spi.device.event.IDeviceCommandInvocation;
import com.sitewhere.spi.device.provisioning.ICommandDeliveryProvider;
import com.sitewhere.spi.device.provisioning.ICommandExecutionBuilder;
import com.sitewhere.spi.device.provisioning.ICommandExecutionEncoder;
import com.sitewhere.spi.device.provisioning.ICommandProcessingStrategy;
import com.sitewhere.spi.device.provisioning.ICommandTargetResolver;
import com.sitewhere.spi.device.provisioning.IDeviceEventProcessor;
import com.sitewhere.spi.device.provisioning.IDeviceProvisioning;

/**
 * Default implementation of the {@link IDeviceProvisioning} interface.
 * 
 * @author Derek
 */
public class DefaultDeviceProvisioning implements IDeviceProvisioning {

	/** Static logger instance */
	private static Logger LOGGER = Logger.getLogger(DefaultDeviceProvisioning.class);

	/** Configured command execution builder */
	private ICommandExecutionBuilder commandExecutionBuilder = new DefaultCommandExecutionBuilder();

	/** Configured command execution encoder */
	private ICommandExecutionEncoder commandExecutionEncoder;

	/** Configured command target resolver */
	private ICommandTargetResolver commandTargetResolver = new DefaultCommandTargetResolver();

	/** Configured command delivery provider */
	private ICommandDeliveryProvider commandDeliveryProvider = new DefaultCommandDeliveryProvider();

	/** Configured command processing strategy */
	private ICommandProcessingStrategy commandProcessingStrategy = new DefaultCommandProcessingStrategy();

	/** Configured list of device event processors */
	private List<IDeviceEventProcessor> deviceEventProcessors = new ArrayList<IDeviceEventProcessor>();

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.sitewhere.spi.ISiteWhereLifecycle#start()
	 */
	@Override
	public void start() throws SiteWhereException {
		LOGGER.info("Starting device provisioning...");

		// Start command execution builder.
		if (getCommandExecutionBuilder() == null) {
			throw new SiteWhereException("No command execution builder configured for provisioning.");
		}
		getCommandExecutionBuilder().start();

		// Start command execution encoder.
		if (getCommandExecutionEncoder() == null) {
			throw new SiteWhereException("No command execution encoder configured for provisioning.");
		}
		getCommandExecutionEncoder().start();

		// Start command target resolver.
		if (getCommandTargetResolver() == null) {
			throw new SiteWhereException("No command target resolver configured for provisioning.");
		}
		getCommandTargetResolver().start();

		// Start command delivery provider.
		if (getCommandDeliveryProvider() == null) {
			throw new SiteWhereException("No command delivery provider configured for provisioning.");
		}
		getCommandDeliveryProvider().start();

		// Start command processing strategy.
		if (getCommandProcessingStrategy() == null) {
			throw new SiteWhereException("No command processing strategy configured for provisioning.");
		}
		getCommandProcessingStrategy().start();

		// Start device event processors.
		if (getDeviceEventProcessors() != null) {
			for (IDeviceEventProcessor processor : getDeviceEventProcessors()) {
				processor.start();
			}
		}

		LOGGER.info("Started device provisioning.");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.sitewhere.spi.ISiteWhereLifecycle#stop()
	 */
	@Override
	public void stop() throws SiteWhereException {
		LOGGER.info("Stopping device provisioning...");

		// Stop command execution builder.
		if (getCommandExecutionBuilder() != null) {
			getCommandExecutionBuilder().stop();
		}

		// Stop command execution encoder.
		if (getCommandExecutionEncoder() != null) {
			getCommandExecutionEncoder().stop();
		}

		// Stop command target resolver.
		if (getCommandTargetResolver() != null) {
			getCommandTargetResolver().stop();
		}

		// Stop command delivery provider.
		if (getCommandDeliveryProvider() != null) {
			getCommandDeliveryProvider().stop();
		}

		// Stop command processing strategy.
		if (getCommandProcessingStrategy() != null) {
			getCommandProcessingStrategy().stop();
		}

		// Stop device event processors.
		if (getDeviceEventProcessors() != null) {
			for (IDeviceEventProcessor processor : getDeviceEventProcessors()) {
				processor.stop();
			}
		}

		LOGGER.info("Stopped device provisioning.");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.sitewhere.spi.device.provisioning.IDeviceProvisioning#deliver(com.sitewhere
	 * .spi.device.event.IDeviceCommandInvocation)
	 */
	@Override
	public void deliver(IDeviceCommandInvocation invocation) throws SiteWhereException {
		getCommandProcessingStrategy().deliver(this, invocation);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.sitewhere.spi.device.provisioning.IDeviceProvisioning#getCommandExecutionBuilder
	 * ()
	 */
	public ICommandExecutionBuilder getCommandExecutionBuilder() {
		return commandExecutionBuilder;
	}

	public void setCommandExecutionBuilder(ICommandExecutionBuilder commandExecutionBuilder) {
		this.commandExecutionBuilder = commandExecutionBuilder;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.sitewhere.spi.device.provisioning.IDeviceProvisioning#getCommandExecutionEncoder
	 * ()
	 */
	public ICommandExecutionEncoder getCommandExecutionEncoder() {
		return commandExecutionEncoder;
	}

	public void setCommandExecutionEncoder(ICommandExecutionEncoder commandExecutionEncoder) {
		this.commandExecutionEncoder = commandExecutionEncoder;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.sitewhere.spi.device.provisioning.IDeviceProvisioning#getCommandTargetResolver
	 * ()
	 */
	public ICommandTargetResolver getCommandTargetResolver() {
		return commandTargetResolver;
	}

	public void setCommandTargetResolver(ICommandTargetResolver commandTargetResolver) {
		this.commandTargetResolver = commandTargetResolver;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.sitewhere.spi.device.provisioning.IDeviceProvisioning#getCommandDeliveryProvider
	 * ()
	 */
	public ICommandDeliveryProvider getCommandDeliveryProvider() {
		return commandDeliveryProvider;
	}

	public void setCommandDeliveryProvider(ICommandDeliveryProvider commandDeliveryProvider) {
		this.commandDeliveryProvider = commandDeliveryProvider;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.sitewhere.spi.device.provisioning.IDeviceProvisioning#getCommandProcessingStrategy
	 * ()
	 */
	public ICommandProcessingStrategy getCommandProcessingStrategy() {
		return commandProcessingStrategy;
	}

	public void setCommandProcessingStrategy(ICommandProcessingStrategy commandProcessingStrategy) {
		this.commandProcessingStrategy = commandProcessingStrategy;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.sitewhere.spi.device.provisioning.IDeviceProvisioning#getDeviceEventProcessors
	 * ()
	 */
	public List<IDeviceEventProcessor> getDeviceEventProcessors() {
		return deviceEventProcessors;
	}

	public void setDeviceEventProcessors(List<IDeviceEventProcessor> deviceEventProcessors) {
		this.deviceEventProcessors = deviceEventProcessors;
	}
}